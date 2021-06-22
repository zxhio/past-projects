//
// Created by zxh on 18-4-4.
//

#ifndef TINYHTTP_TINY_H
#define TINYHTTP_TINY_H

#include <wait.h>
#include <sys/socket.h>

#define MAXLINE 4096

// Unix IO wrappers
int Open(const char *pathname, int flags, mode_t mode);
void Close(int fd);
int Dup2(int fd1, int fd2);


// Process control wrappers
pid_t Fork(void);
void Execve(const char *filename, char *const argv[], char *const envp[]);
pid_t Wait(int *status);
pid_t Waitpid(pid_t pid, int *iptr, int options);
void Kill(pid_t pid, int signum);
unsigned int Sleep(unsigned int secs);
void Pause(void);
unsigned int Alarm(unsigned int seconds);
void Setpgid(pid_t pid, pid_t gpid);
pid_t Getpgrp();

// Wrappers for memory mapping functions
void *Mmap(void *addr, size_t len, int port, int flags, int fd, off_t offest);
void Munmap(void *start, size_t length);

// Sockets interface wrappers
int Socket(int domain, int type, int protocol);
void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
void Bind(int sockfd, struct sockaddr *my_addr, int addlen);
void Listen(int s, int backlog);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);

// DNS wrappers
struct hostent *Gethostbyname(const char *name);
struct hostent *Gethostbyaddr(const char *addr, int len, int type);

// Client/Server helper functions
int open_clientfd(char *hostname, int portno);
int open_listenfd(int portno);

// Wrappers for client/server helper functions
int Open_clientfd(char *hostname, int port);
int Open_listenfd(int port);

#endif //TINYHTTP_TINY_H
