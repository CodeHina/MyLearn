#ifndef __SERVER_H_
#define __SERVER_H_

#include <arpa/inet.h>
#include <sys/epoll.h>

#include "./inet_const_val.h"
#include "./epoll_func.h"


class Server
{
    int listen_fd;
    int communicate_fd;
    int epfd;           //epoll_fd - 指向内部的红黑树
    

    sockaddr_in* sevr_addr;
    sockaddr_in* clet_addr;
    

public:

    epoll_event* pevents;
    epoll_data_func edata_func;   

    char str_addr[INET_ADDRSTRLEN];

    Server() : listen_fd(-1), communicate_fd(-1), epfd(-1) {  }
    ~Server() { delete sevr_addr; delete clet_addr;  }


    void set_config();   //配置服务器信息

    void wait_login();   //等待客户端连接

    int& get_epfd()
    { return epfd; }
    
    int get_listen_fd()
    { return listen_fd; }

    int get_comm_fd()
    { return communicate_fd; }
};


int Epoll_create(int size, int& epfd);

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event* events);

int Epoll_wait(int epfd, struct epoll_event events[], int maxevents, int timeout);

void handle_epevents(int, void*);
    





#endif
