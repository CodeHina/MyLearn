#include <iostream>
#include <array>
#include <cstring>
#include <cerrno>
#include <cctype>       //for toupper()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include "../include/wrap_sock.h"
#include "../include/inet_const_val.h"

using std::array;



int main()
{
    int listen_fd = -1, commucate_fd = -1; //连接文件描述符 - 通信文件描述符

    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sevr_addr;
    memset(&sevr_addr, 0, sizeof(sevr_addr));
    sevr_addr.sin_family = AF_INET;
    sevr_addr.sin_port = htons(SEVR_PORT);
    sevr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //uint32_t INADDR; inet_pton(AF_INET, SEVR_IP_ADDR, &INADDR);  - INADDR即为所规定的本地地址

    Bind(listen_fd, (sockaddr*)&sevr_addr, sizeof(sevr_addr));

    Listen(listen_fd, 128);

    //设置fd_set

    fd_set all_rdsets;
    FD_ZERO(&all_rdsets);
    FD_SET(listen_fd, &all_rdsets);        //将监听连接的文件描述符添加到read_sets - 客户端请求连接是写数据

    int monitor_fd[FD_SETSIZE];            //都置为-1
    for(int i = 0; i < FD_SETSIZE; ++i)
        monitor_fd[i] = -1;

    size_t max_i = -1;                      //最大文件描述符编号对应的下标 

    int maxfds = listen_fd;                //最大文件描述符编号
    int ready_fds = 0;
    
                                        
    char str_addr[INET_ADDRSTRLEN]; /* #define INET_ADDRSTRLEN 16 */ //存储客户端ip

    while(1)
    {
        fd_set read_sets = all_rdsets;      /* 外部的备份all_rdsets来初始化所属的监听读文件描述符集 */

again:
        ready_fds = select(maxfds + 1, &read_sets, nullptr, nullptr, nullptr);


        if(ready_fds < 0)
        {
            if(ready_fds == -1 && errno == EINTR)
                goto again;
            else if(ready_fds == -1)
            { perror("select err"); exit(-1);}
        }
        else //当前已有文件描述符准备好I/O
        {
            if(FD_ISSET(listen_fd, &read_sets)) //监听的客户端的连接的文件描述符已有数据可读
            {
                sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                commucate_fd = Accept(listen_fd, (sockaddr*)&client_addr, &client_addr_len);    /* 这里是非阻塞的 */
                

                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, str_addr, sizeof(str_addr));

                std::cout << "client ip : " << str_addr                             //打印登录的客户端信息
                          << " : " << ntohs(client_addr.sin_port) << " log in " << std::endl;
                Write(commucate_fd, "---welcome!!---", strlen("---welcome!!---"));  //同时向客户端发送欢迎信息

/* 需要将当前得到commucate_fd和文件描述符相关的数据都更新一遍 -- 非常重要 */

                /* 将当前新增的文件描述符添加到所需的readfds/writefds/exceptfds中 */
                FD_SET(commucate_fd, &all_rdsets);  //加到读监听集合
                /* 更新当前monitor_fd - 将得到的commucate_fd添加至此 */
                size_t i = 0;
                while(i < FD_SETSIZE) //加到文件描述符监听数组
                {
                    if(monitor_fd[i] == - 1)
                    {
                        monitor_fd[i] = commucate_fd; 
                        break;
                    }   
                    ++i;
                }
                /* 更新 max_i */
                if(i > max_i)
                    max_i = i;
                /* 更新 maxfds */
                if(commucate_fd > maxfds)
                    maxfds = commucate_fd;
                /* 当前已到达监控上限 - 1024 - 故出错处理 */
                if(i == FD_SETSIZE)
                {
                    std::cerr << "too many clients over FD_SETSIZE" << std::endl;
                    exit(-1);
                }
                
                /* 当前就绪文件描述符-1 - 并判断当前是否处理完了 */
                if(--ready_fds == 0)
                    continue;           //直接跳出 - 进行新的处理
            }
            
/* 检查所有的监听的文件描述符 - 判断那个文件描述符能够I/O */
            int sockfd = -1;
            for(size_t i = 0; i <= max_i; ++i) 
            {
                if( (sockfd = monitor_fd[i]) == -1 )
                    continue; //跳过当前无效的文件描述符

                if( FD_ISSET(sockfd, &read_sets) )
                {
                    char buf[BUFSIZ];
                    int rdbytes = -1;
    

                    /* 当前文件已经读完 - 或者客户端已经关闭 */
                    if( (rdbytes = Read(sockfd, buf, sizeof(buf) )) == 0 )
                    {
                        Close(sockfd);                  //客户端请求关闭连接 - 四次挥手断开两端连接
                        FD_CLR(sockfd, &all_rdsets);    //将断开连接socket从文件描述符集合all_rdsets中去除
                        /*更新 monitor_fd 数组*/
                        monitor_fd[i] = -1;         //不再使用即置为-1
                        
                        std::cout << "a client log out" << std::endl;

                    }
                    else //正常处理文件
                    {
                        std::cout << "the msg from client " << str_addr << " is " << buf << std::endl;
                        
                        for(size_t i = 0; i < strlen(buf); ++i)
                        {
                            buf[i] = toupper(buf[i]);
                        }

                        Write(sockfd, buf, strlen(buf));
                    }
                    /* 当前处理的文件描述符已完毕 - 即-1 */
                    if(--ready_fds == 0)
                        break;           //直接跳出for循环 - 进行新的处理
                }
            }

        }
    
    }

    Close(listen_fd);
    
    return 0;
}
