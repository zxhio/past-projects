//
// Created by zxh on 18-4-5.
//

#include "error.h"
#include "rio.h"
#include "tiny.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define __USE_GNU       // use 'environ'
#include <unistd.h>
#include <netdb.h>

void doit(int fd);
void read_request_header(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void server_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void server_dynamic(int fd, char *filename, char *cgiargs);
void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
	int listenfd, connfd, port;
	char hostname[MAXLINE];
	struct sockaddr_in clientaddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);
	listenfd = open_listenfd(port);
	socklen_t clientlen = sizeof(clientaddr);
	while (1) {
		connfd = Accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		getnameinfo((struct sockaddr *)&clientaddr,clientlen, hostname, MAXLINE, argv[1], MAXLINE, 0);
		printf("Accepted connection from (%s, %d)\n", hostname, port);
		if (Fork() == 0)	// child process
			doit(connfd);
		Close(connfd);
		Wait(NULL);
	}
}

void doit(int fd)
{
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;

	rio_readinitb(&rio, fd);
	rio_readlineb(&rio, buf, MAXLINE);
	sscanf(buf, "%s %s %s", method, uri, version);
	if (strcasecmp(method, "GET") != 0) {
		client_error(fd, method, "501", "Not Implemented", "Tiny does not implement this method");
		return;
	}
	read_request_header(&rio);

	// Parse URI from GET request
	is_static = parse_uri(uri, filename, cgiargs);
	if (stat(filename, &sbuf) < 0) {
		client_error(fd, filename, "404", "Not Found", "Tiny couldn't find this file");
		return;
	}

	if (is_static) {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
			client_error(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
			return;
		}
		server_static(fd, filename, sbuf.st_size);
	} else {
		if (!(S_ISREG(sbuf.st_mode) || !(S_IXUSR & sbuf.st_mode))) {
			client_error(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
			return;
		}
		server_dynamic(fd, filename, cgiargs);
	}
}

void read_request_header(rio_t *rp)
{
	char buf[MAXLINE];

	rio_readlineb(rp, buf, MAXLINE);
	while (strcmp(buf, "\r\n")) {
		rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
	char *p;

	if (!strstr(uri, "cgi-bin")) {
		strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri)-1] == '/')
			strcat(filename, "home.html");
		return 1;
	} else {
		p = index(uri, '?');
		if (p) {
			strcpy(cgiargs, p+1);
			*p = '\0';
		} else {
			strcpy(cgiargs, "");
		}
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}

void get_filetype(char *filename, char *filetype)
{
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else
		strcpy(filetype, "text/plain");
}

void server_static(int fd, char *filename, int filesize)
{
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXLINE];

	get_filetype(filename, filetype);
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sServer: Tiny Http Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	rio_writen(fd, buf, strlen(buf));
	printf("Response headers:\n");
	printf("%s", buf);

	srcfd = Open(filename, O_RDONLY, 0);
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	Close(srcfd);
	rio_writen(fd, srcp, filesize);
	munmap(srcp, filesize);
}

void server_dynamic(int fd, char *filename, char *cgiargs)
{
	char buf[MAXLINE], *emptylist[] = { NULL };

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny HTTP Server\r\n");
	rio_writen(fd, buf, strlen(buf));

	if (Fork() == 0) {
		setenv("QUERY_STRING", cgiargs, 1);
		Dup2(fd, STDOUT_FILENO);
		Execve(filename, emptylist, environ);
	}
	Wait(NULL);
}

void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
	char buf[MAXLINE], body[MAXLINE];

	/* Build the HTTP response body */
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

	/* Print the HTTP response */
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	rio_writen(fd, buf, strlen(buf));
	rio_writen(fd, body, strlen(body));
}