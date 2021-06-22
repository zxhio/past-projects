//
// Created by zxh on 18-4-5.
//

#include "tiny.h"
#include "error.h"
#include <unistd.h>
#include <sys/mman.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>


// Unix IO wrappers
int Open(const char *pathname, int flags, mode_t mode)
{
	int rc;

	if ((rc = open(pathname, flags, mode)) < 0)
		err_sys("Open error");
	return rc;
}

void Close(int fd)
{
	int rc;

	if ((rc = close(fd)) < 0)
		err_sys("Close error");
}

int Dup2(int fd1, int fd2)
{
	int rc;

	if ((rc = dup2(fd1, fd2)) < 0)
		err_sys("Dup2 error");
	return rc;
}

// Process control wrappers
pid_t Fork(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("Fork error");
	return pid;
}

void Execve(const char *filename, char *const argv[], char *const envp[])
{
	if (execve(filename, argv, envp) < 0)
		err_sys("Execve error");
}

pid_t Wait(int *status)
{
	pid_t pid;

	if ((pid = wait(status)) < 0)
		err_sys("Wait error");
	return pid;
}

pid_t Waitpid(pid_t pid, int *iptr, int options)
{
	pid_t retpid;

	if ((retpid = waitpid(pid, iptr, options)) < 0)
		err_sys("Waitpid error");
	return retpid;
}

void Kill(pid_t pid, int signum)
{
	int rc;

	if ((rc = kill(pid, signum)) < 0)
		err_sys("Kill error");
}

void Pause()
{
	(void)pause();
	return;
}

unsigned int Sleep(unsigned int secs)
{
	int rc;

	if ((rc = sleep(secs)) < 0)
		err_sys("Sleep error");
	return rc;
}

unsigned int Alarm(unsigned int seconds)
{
	return alarm(seconds);
}

void Setpgid(pid_t pid, pid_t pgid)
{
	int rc;

	if ((rc = setpgid(pid, pgid)) < 0)
		err_sys("Setpgid error");
	return;
}

// Wrappers for memory mapping functions

void *Mmap(void *addr, size_t len, int port, int flags, int fd, off_t offest)
{
	void *ptr;

	if ((ptr = mmap(addr, len, port, flags, fd, offest)) == ((void *)-1))
		err_sys("Mmap error");
	return ptr;
}

void Munmap(void *start, size_t length)
{
	if (munmap(start, length) < 0)
		err_sys("Munmap error");
}



// Sockets interface wrappers
int Socket(int domain, int type, int protocol)
{
	int rc;

	if ((rc = socket(domain, type, protocol)) < 0)
		err_sys("Socket error");
	return rc;
}

void Setsockopt(int s, int level, int optname, const void *optval, int optlen)
{
	int rc;

	if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
		err_sys("Setsockopt erro");
}

void Bind(int sockfd, struct sockaddr *my_addr, int addlen)
{
	int rc;

	if ((rc = bind(sockfd, my_addr, addlen)) < 0)
		err_sys("Bind error");
}

void Listen(int s, int backlog)
{
	int rc;

	if ((rc = listen(s, backlog)) < 0)
		err_sys("Listen error");
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	int rc;

	if ((rc = accept(s, addr, addrlen)) < 0)
		err_sys("Accept error");
	return rc;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
	int rc;

	if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
		err_sys("Connect error");
}

// DNS interface wrappers
struct hostent *Gethostbyname(const char *name)
{
	struct hostent *p;

	if ((p = gethostbyname(name)) == NULL)
		err_sys("DNS Error: Gethostbyname error");
	return p;
}

struct hostent *Gethostbyaddr(const char *addr, int len, int type)
{
	struct hostent *p;

	if ((p = gethostbyaddr(addr, len, type)) == NULL)
		err_sys("DNS Error: Gethostbyaddr error");
	return p;
}

// Client/server helper functions
int open_clientfd(char *hostname, int port)
{
	int clienfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;

	if ((clienfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// fill in the server's IP address and port
	if ((hp = gethostbyname(hostname)) == NULL)
		return -2;
	memset((char *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	memcpy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = port;

	// establish a connection with the server
	if (connect(clienfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	return clienfd;
}

int open_listenfd(int port)
{
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// eliminates "address already in use" error from bind
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
		return -1;

	memset((char *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0, listen any local address
	serveraddr.sin_port = htons((unsigned short)port);

	if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	if (listen(listenfd, 128) < 0)
		return -1;
	return listenfd;
}