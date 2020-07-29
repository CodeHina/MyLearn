#ifndef __WRAP_SOCK_H_
#define __WRAP_SOCK_H_

/* 将常用的网络socket编程所需的函数进行封装出错处理 */
#include <unistd.h>

int Socket(int domain, int type, int protocol);

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Listen(int sockfd, int backlog);

int Accept(int socket, struct sockaddr* address/*O*/, socklen_t* address_len/*I/O*/);

int Connect(int sockfd, const struct sockaddr *addr/*o*/, socklen_t addrlen);

ssize_t Read(int fd, void *buf/*O*/, size_t count);

ssize_t Write(int fd, const void* buf, size_t count);

int Close(int fd);

ssize_t Readn(int fd, void* buf, size_t nbytes);        //一次读取指定的nbytes
    
ssize_t Writen(int fd, const void* buf, size_t nbytes); //一次写入指定的nbytes

ssize_t my_read(int fd, char* ptr);                     //一次向ptr指向的缓冲区读入多少字节

ssize_t ReadLine(int fd, void* buf, size_t maxlen);     //一次从buf中读入一行数据





#endif
