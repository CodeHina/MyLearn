#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../include/wrap_sock.h"
#include "../include/sys_err.h"


int Socket(int domain, int type, int protocol)
{
    int sockfd = socket(domain, type, protocol);
    if(sockfd == -1)
        perr_exit("socket");
    
    return sockfd;
}


int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret = bind(sockfd, addr, addrlen);
    
    if(ret == -1)
        perr_exit("bind");

    return ret;
}


int Listen(int sockfd, int backlog)
{
    int ret = listen(sockfd, backlog);
    
    if(ret == -1)
        perr_exit("listen");

    return ret;
}

  /* accept()属于慢速系统调用 - 可能会被信号中断 - 产生EINTR错误 */
int Accept(int socket, struct sockaddr* address/*O*/, socklen_t* address_len/*I/O*/)
{
    int ret = accept(socket, address, address_len);

again:
    if(ret == -1)
    {
        if(errno == ECONNABORTED || errno == EINTR)
            goto again; //上述错误连接和信号中断错误即重启
        else
            perr_exit("accept");
    }

    return ret;
}


int Connect(int sockfd, const struct sockaddr *addr/*o*/, socklen_t addrlen)
{
    int ret = connect(sockfd, addr, addrlen);
    
    if(ret == -1)
        perr_exit("connect");
    
    return ret;
}

  /* read()也是慢速系统调用 - 会阻塞等待 */
ssize_t Read(int fd, void *buf/*O*/, size_t count)
{
    ssize_t rdbytes = read(fd, buf, count);

again:
    if(rdbytes == -1)
    {
        if(errno == EINTR)
            goto again;
        else
            perr_exit("read");
    }
    return rdbytes;
}


ssize_t Write(int fd, const void* buf, size_t count)
{
    int wrbytes = write(fd, buf, count);

again:
    if(wrbytes == -1)
    {
        if(errno == EINTR)
            goto again;
        else
            perr_exit("write");
    }
    
    return wrbytes;
}


int Close(int fd)
{
    int ret = close(fd);
    if(ret == -1)
        perr_exit("close");
    return ret;
}


ssize_t Readn(int fd, void* buf, size_t nbytes)        //一次读取指定的nbytes到buf
{
    //网络的数据包最大1500字节,read()函数读完就会返回0
    //不能满足要求一次读取4096这样的要求 - 故封装一个函数
    size_t left_bytes = nbytes; //剩余要读的字节数
    size_t read_bytes = -1;     //读取的字节数
    char* sbuf = (char*)buf;

    while(left_bytes > 0)
    {
        if( (read_bytes = read(fd, sbuf, left_bytes)) < 0 )
        {
            if(errno == EINTR)
                read_bytes = 0;
            else
                perr_exit("readn err");
        }
        
        left_bytes -= read_bytes;   //更新剩余字节以及下次读取位置
        sbuf += read_bytes; 
    }

    return nbytes - left_bytes;
}

ssize_t Writen(int fd, const void* buf, size_t nbytes) //一次写入指定的nbytes
{
    size_t left_bytes = nbytes;
    size_t write_bytes = -1;
    char* sbuf = (char*)buf;

    while(left_bytes > 0)
    {
        if( (write_bytes = write(fd, sbuf, left_bytes)) < 0 )
        {
            if(errno == EINTR)
                write_bytes = 0;
            else
                perr_exit("writen err");
        }

        left_bytes -= write_bytes;
        sbuf += write_bytes;
    }

    return nbytes - left_bytes; 
}

ssize_t my_read(int fd, char* ptr)                     //一次向ptr指向的缓冲区读入多少字节
{

    return 0;
}

ssize_t ReadLine(int fd, void* buf, size_t maxlen)     //一次从buf中读入一行数据
{

    return 0;
}
