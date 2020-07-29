#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>

#include "../include/wrap_sock.h"
#include "../include/inet_const_val.h"

const int MAX_OPEN = 500; //ulimit -a  - open files(65535)

int main()
{   

    int lsen_fd = -1, cnect_fd = -1;

    char str_addr[INET_ADDRSTRLEN];

    /* set inet config */
    lsen_fd = Socket(AF_INET, SOCK_STREAM, 0);

    /* 监听客户端连接请求的lsen_fd设置为非阻塞 */
    int flag = fcntl(lsen_fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(lsen_fd, F_SETFL, flag);

    sockaddr_in sevr_addr;
    sevr_addr.sin_family = AF_INET;
    sevr_addr.sin_port = htons(SEVR_PORT);
    sevr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(lsen_fd, (sockaddr*)&sevr_addr, sizeof(sevr_addr));
    Listen(lsen_fd, 128);

    /* epoll and accept */
    epoll_event register_evt;//待注册事件
    epoll_event *ret_evt = new epoll_event[MAX_OPEN];//epoll最多监控的文件描述符个数
    memset(ret_evt, -1, MAX_OPEN);

    int epfd = epoll_create(MAX_OPEN);
    register_evt.events = EPOLLIN; //边沿触发epoll_wait
    register_evt.data.fd = lsen_fd;
    
    epoll_ctl(epfd, EPOLL_CTL_ADD, lsen_fd, &register_evt); //lsen_fd已添加到epfd指向的红黑树

    
    while(1)
    {
        /* 阻塞等待注册的文件描述符产生事件 - 返回时填充ret_evt */
        int ready_fds = epoll_wait(epfd, ret_evt, MAX_OPEN, -1);
        
        std::cout << "epoll wait start" << std::endl;

        if(ready_fds == -1)
            perror("epoll_wait err");

        for(int i = 0; i < ready_fds; ++i)
        {
            if( !(ret_evt[i].events & EPOLLIN) )
                continue;//当前非读事件 - 暂时不处理
            
            if( ret_evt[i].data.fd == lsen_fd ) //客户端的连接请求
            {
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_len);
                cnect_fd = Accept(lsen_fd, (sockaddr*)&client_addr, &client_len);

                /* 此客户端和服务端通信的文件描述符设置为非阻塞 */
                //int flag = fcntl(cnect_fd, F_GETFL);
                //flag |= O_NONBLOCK;
                //fcntl(cnect_fd, F_SETFL, flag);
                /* 此文件描述符监听事件为边沿触发的读 - 注册到epfd的红黑树上 */
                register_evt.events = EPOLLIN;
                register_evt.data.fd = cnect_fd;
                int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cnect_fd, &register_evt);
                if(ret == -1)
                    perror("epoll_ctl err");
                
                const char* addr =  inet_ntop(AF_INET, &(client_addr.sin_addr), str_addr, sizeof(str_addr));
                std::cout << addr << ":" << ntohs(client_addr.sin_port)
                          << " log in" << std::endl;
                    
                Write(cnect_fd, "welcom!!!", 10);
            }
            else //非客户端的连接请求的 - 可读事件
            {   
                int sockfd = ret_evt[i].data.fd; //取出当前发生读事件的文件描述符
                
                const int RD_BUFSIZ = 20;
                char buf[RD_BUFSIZ];

                int rd_bytes = Read(sockfd, buf, RD_BUFSIZ/2);

                std::cout << "rd_bytes - " << rd_bytes << std::endl;
                
                /*
                while(rd_bytes > 0)
                {
                    for(int i = 0; i < rd_bytes; ++i)
                        buf[i] = toupper(buf[i]);

                    Write(sockfd, buf, rd_bytes);

                    rd_bytes = Read(sockfd, buf, RD_BUFSIZ/2);

                    if(rd_bytes == 0)
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &register_evt);
                        Close(sockfd);
                    }
                }
                */
                if(rd_bytes == 0)//写端关闭
                {
                    std::cout << "a client log out" << std::endl;

                    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                
                    Close(sockfd);

                }
                else 
                {
                    std::cout << "read msg is " <<  buf << std::endl;
                    for(int i = 0; i < rd_bytes; ++i)
                        buf[i] = toupper(buf[i]);

                    Write(sockfd, buf, rd_bytes);
                }
            }


        }
    }

    Close(lsen_fd);

    delete []ret_evt;
    return 0;
}


