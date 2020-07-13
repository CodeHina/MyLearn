#include <iostream>
#include <cstring>
#include <cerrno>
#include <cctype>       //for toupper()
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#include "../include/wrap_sock.h"
#include "../include/inet_const_val.h"

#define OPEN_MAX 5000   //poll最大打开文件描述符个数

//int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);
//参数1 - pollfd[] - 一个关心的文件描述符信息的数组
/*
   struct pollfd{
   int fd;
   short events;   //监视的事件 - POLLIN(文件描述符有写入的数据 - 监视读)/POLLOUT/POLLERR
   short revents;  //内核填充此参数 - 表示发生的事件
   } 
   */
//参数2 - nfds - 所要监视的文件描述符的个数 - 可以利用下标加+1来计算
//参数3 - -1 == 阻塞等待 / 0 - 立即返回 - >0 规定事件返回
//返回值 - >0 产生事件的文件描述符的总个数 - 0 无事件产生 - -1 错误
//由于poll和select都只是返回发生事件的文件描述符总数 - 故需要进行循环数组找到满足条件的文件描述符

int main()
{
    int listen_fd = -1, commucate_fd = -1;

    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sevr_addr;
    memset(&sevr_addr, 0, sizeof(sevr_addr));
    sevr_addr.sin_family = AF_INET;
    sevr_addr.sin_port = htons(SEVR_PORT);
    sevr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listen_fd, (sockaddr*)&sevr_addr, sizeof(sevr_addr));

    Listen(listen_fd, 128);

    struct pollfd monitor_fds[OPEN_MAX];         //所要监听的文件描述符
    memset(monitor_fds, -1, sizeof(monitor_fds));//置为-1表示未使用

    pollfd m_listen_fd;
    m_listen_fd.fd = listen_fd;
    m_listen_fd.events = POLLIN;            //监听客户端连接请求

    monitor_fds[0] = m_listen_fd;           //放入监听链表中
    size_t max_i = 0;                       //当前监听链表监听的文件描述符的最大值下标

    size_t ready_fds = -1;                  //就绪的文件描述符个数

    size_t ifd = 0;                         //监测数组下标

    while(1)
    {
        char str_addr[INET_ADDRSTRLEN];
        
        ready_fds = poll(monitor_fds, max_i + 1, -1);
        if(ready_fds <= 0)
        {
            if(ready_fds == 0 || errno == EINTR)
                continue;
            else
            { perror("poll err"); exit(-1); }
        }
        else //当前监听的文件描述符有事件发生
        {
            if( monitor_fds[0].revents & POLLIN ) //有客户端的连接请求发送过来 - 由于是位图采用&操作来判断
            {

                sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                commucate_fd = Accept(listen_fd, (sockaddr*)&client_addr, &client_addr_len);    /* 这里是非阻塞的 */

                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, str_addr, sizeof(str_addr));

                std::cout << "client ip : " << str_addr                             //打印登录的客户端信息
                    << " : " << ntohs(client_addr.sin_port) << " log in " << std::endl;
                Write(commucate_fd, "---welcome!!---", strlen("---welcome!!---"));  //同时向客户端发送欢迎信息

                pollfd mon_fd;

                size_t i = 1;//[1, OPEN_MAX) 找到空闲位置
                for(i = 1; i < OPEN_MAX; ++i)
                {
                    if(monitor_fds[i].fd == -1) //当前空格无存储
                    { monitor_fds[i] = mon_fd; break; }
                }

                if(i == OPEN_MAX)
                { std::cerr << "too many client" << std::endl; exit(-1); }

                /* 更新新增的comuncate_fd相关信息 */
                monitor_fds[i].fd = commucate_fd;
                monitor_fds[i].events = POLLIN; //监听事件为读
                /* 更新当前有效文件描述符的最大下标 */
                if(i > max_i)
                    max_i = i;

                if(--ready_fds == 0) //当前事件已经处理完毕
                    continue;        //跳出循环 - 再次监听
            }


            //处理非客户端请求的事件 - 遍历所有可能有效的文件描述符
            int sockfd;
            for(ifd = 1; ifd < max_i + 1; ++ifd)
            {
                if( (sockfd = monitor_fds[ifd].fd) == -1 ) //无效的文件描述符
                    continue;                              //考察下一个

                if( monitor_fds[ifd].revents & POLLIN)      //当前事件为可读
                {
                    char buf[BUFSIZ];
                    size_t rdbytes;

                    if( (rdbytes = Read(sockfd, buf, sizeof(buf))) < 0 )//读取发生错误
                    {
                        if(errno == ECONNRESET) //读取的socket被强制关闭
                        {
                            std::cerr << "client " << ifd << " aborted connection" << std::endl;
                            Close(sockfd); 
                            monitor_fds[ifd].fd = -1; //将此文件描述符置为无效
                        }
                    }
                    else if(rdbytes == 0) //当前客户端要请求关闭 - 读取数据为0
                    {
                        std::cout << "client " << ifd << " log out" << std::endl;
                        Close(sockfd);
                        monitor_fds[ifd].fd = -1;   //将此文件描述符置为无效
                    }
                    else //当前为正常通信
                    {
                        std::cout << "the msg from client " << str_addr << " is " << buf << std::endl;

                        for(size_t i = 0; i < strlen(buf); ++i)
                        {
                            buf[i] = toupper(buf[i]);
                        }

                        Write(sockfd, buf, strlen(buf));
                    }

                    /* 处理其他事件 */


                    //当前事件已经处理完毕
                    if(--ready_fds == 0)
                        break;

                }

            }

        }

    }

    return 0;
}
