#include <cstring>
#include <iostream>

#include "../include/wrap_sock.h"
#include "../include/server.h"


void Server::set_config()
{
    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in* sevr_addr = new sockaddr_in;
    memset(&sevr_addr, 0, sizeof(sevr_addr));
    sevr_addr->sin_family = AF_INET;
    sevr_addr->sin_port = htons(1234);
    sevr_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listen_fd, (sockaddr*)sevr_addr, sizeof(sevr_addr));

    Listen(listen_fd, 128);

}

void Server::wait_login()
{
    clet_addr = new sockaddr_in;
    socklen_t clet_len = sizeof(sockaddr_in);

    communicate_fd = Accept(listen_fd, (sockaddr*)clet_addr, &clet_len);

    inet_ntop(AF_INET, &clet_addr->sin_addr.s_addr, str_addr, sizeof(str_addr));

    std::cout << "client ip : " << str_addr                             //打印登录的客户端信息
        << " : " << ntohs(clet_addr->sin_port) << " log in " << std::endl;

    Write(communicate_fd, "---welcome!!---", strlen("---welcome!!---"));  //同时向客户端发送欢迎信息
}


int Epoll_create(int size, int& epfd)
{
    epfd = epoll_create(size);
    if(epfd == -1)
    {
        perror("epoll_create err");
        exit(-1);
    }  

    return epfd;
}

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event* events)
{
    int ret = epoll_ctl(epfd, op, fd, events);
    if(ret == -1)
    {
        perror("epoll_ctl err");
        exit(-1);
    }  
    return ret;
}

int Epoll_wait(int epfd, struct epoll_event events[], int maxevents, int timeout)
{
    //慢速系统调用 - 可能会被信号中断
again:
    int ret = epoll_wait(epfd, events, maxevents, timeout);

    if(ret == -1)
    {
        if(errno == EINTR)
            goto again;
        else
        {
            perror("epoll_wait err");
            exit(-1);
        }
    }

    return ret;
}

void handle_epevents(int sockfd, void* arg)
{
    Server* sevr = (Server*)arg;
    

    if(sockfd & EPOLLIN)//当前文件描述符的事件为读事件
    {
        char buf[BUFSIZ];
        int rdbytes = -1;
    
        /* 当前接受到客户端的连接请求 */
        if(sevr->get_listen_fd()== sockfd)
        {
            sevr->wait_login();
        }

        /* 当前文件已经读完 - 或者客户端已经关闭 */
        if( (rdbytes = Read(sockfd, buf, sizeof(buf) )) == 0 )
        {
            Close(sockfd);                  //客户端请求关闭连接 - 四次挥手断开两端连接
            /*更新 monitor_fd 数组*/

            std::cout << "a client log out" << std::endl;

        }
        else //正常处理文件
        {
            std::cout << "the msg from client " << sevr->str_addr << " is " << buf << std::endl;

            for(size_t i = 0; i < strlen(buf); ++i)
            {
                buf[i] = toupper(buf[i]);
            }

            Write(sockfd, buf, strlen(buf));
        }

        return; //当前事件处理完毕即返回
    }
    else
    {
        //其他事件 - 文件描述符的写事件就绪等
        
        return;
    }
}


