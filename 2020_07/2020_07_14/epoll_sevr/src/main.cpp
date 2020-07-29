#include "../include/server.h"
#include "../include/wrap_sock.h"
#include <iostream>
#include <cctype>
#include <cstring>

int main(int argc, char* argv[])
{
    Server my_server;

    my_server.set_config();

    my_server.pevents = new epoll_event;

    epoll_event ep_commu_event[OPEN_MAX]; /* 用于填充的结构体 - 保存有事件文件描述符 */

    Epoll_create(OPEN_MAX,my_server.get_epfd());

    Epoll_ctl(my_server.get_epfd(), EPOLL_CTL_ADD, my_server.get_epfd(), my_server.pevents);

    my_server.pevents->data.fd = my_server.get_listen_fd();
    my_server.pevents->events = EPOLLIN;


    while(1)
    {
        int ready_fds = Epoll_wait(my_server.get_epfd(), ep_commu_event, OPEN_MAX, -1);

        for(int i = 0; i < ready_fds; ++i)
        {
            if(!(ep_commu_event[i].events & EPOLLIN)) //当前事件非可读事件
                continue;

            if(ep_commu_event[i].data.fd == my_server.get_listen_fd()) //当前为客户端请求连接
            {
                my_server.wait_login();

                my_server.pevents->data.fd = my_server.get_comm_fd();
                my_server.pevents->events = EPOLLIN;

                Epoll_ctl(my_server.get_epfd(), EPOLL_CTL_ADD, my_server.get_comm_fd(), my_server.pevents);

            }
            else //客户端的通信信息
            {
                int sockfd = ep_commu_event[i].data.fd;

                char buf[1024];
                size_t rdbytes;

                /* 当前文件已经读完 - 或者客户端已经关闭 */
                if( (rdbytes = Read(sockfd, buf, sizeof(buf) )) == 0 )
                {
                    Close(sockfd);                  //客户端请求关闭连接 - 四次挥手断开两端连接

                    std::cout << "a client log out" << std::endl;

                }
                else //正常处理文件
                {
                    std::cout << "the msg from client " << my_server.str_addr << " is " << buf << std::endl;

                    for(size_t i = 0; i < strlen(buf); ++i)
                    {
                        buf[i] = toupper(buf[i]);
                    }

                    Write(sockfd, buf, strlen(buf));
                }

            }

        }



    }

    delete my_server.pevents;

    return 0;
}


