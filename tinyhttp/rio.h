//
// Created by zxh on 18-1-5.
//

#ifndef TINYHTTP_RIO_H
#define TINYHTTP_RIO_H

#include <stdio.h>

// fd -> usrbuf (n bytes)
ssize_t rio_readn(int fd, void *usrbuf, size_t n);

// usrbuf (n bytes) -> fd
ssize_t rio_writen(int fd, void *usrbuf, size_t n);


#define RIO_BUFSIZE     4096

typedef struct {
	int rio_fd;
	int rio_cnt;
	char *rio_bufptr;
	char rio_buf[RIO_BUFSIZE];
} rio_t;

// connect FD to RP
void rio_readinitb(rio_t *rp, int fd);

// read a line(not more than maxlen bytes) from RP(include '\n'),
// and terminate the line with '\0'
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

//read N bytes from RP to USRBUF
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

#endif //TINYHTTP_RIO_H
