#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/wait.h>

const int M_BUF_SIZE = 10;

int main()
{

    char buf[M_BUF_SIZE] = {'0'};
    char c = 'a';

    int pfd[2];

    pipe(pfd);//ret - 0/-1

    pid_t pid = fork();

    if(pid == -1)
    {
        perror("fork");
    }
    else if( pid == 0 ) //子进程写数据
    {
        close(pfd[0]); //关闭读端

        while(1)
        {
            int i = 0;
            for(i = 0; i < M_BUF_SIZE / 2; ++i)
                buf[i] = c;
            buf[i] = '\n';
            c++;

            for(; i < M_BUF_SIZE; ++i)
                buf[i] = c;
            buf[i] = '\n';
            c++;

            write(pfd[1], buf, sizeof(buf));

            sleep(5);
        }
        close(pfd[1]);
    }
    else
    {
        close(pfd[1]); //关闭写端

        struct epoll_event register_evt; //要注册的文件描述符
        struct epoll_event ret_evt[10];  //可能发生事件的epoll_event数组


        register_evt.events = EPOLLIN;         //水平触发epoll_wait - 默认 - 未将此次数据处理完则不断通知
        //register_evt.events = EPOLLIN | EPOLLET; //边沿触发 - 只通知一次 - 是否处理完则不关心
        register_evt.data.fd = pfd[0];   

        int epfd = epoll_create(10);
        epoll_ctl(epfd, EPOLL_CTL_ADD, register_evt.data.fd, &register_evt);


        while(1)
        {
            int ready_evts = epoll_wait(epfd, ret_evt, 10, 0);
            for(int i = 0; i < ready_evts; ++i)
            {
                if(pfd[0] == ret_evt[i].data.fd)//当前响应的事件为pfd[0]的事件
                {
                    read(pfd[0], buf, M_BUF_SIZE / 2); //每次只读缓冲区的一半数据
                    std::cout << "ready_evts : " << ready_evts << " buf now is " << buf << std::endl;
                }
                else
                    continue;
            }
            //sleep(2); //放大现象
        }

        wait(nullptr);//回收子进程
        close(pfd[0]);
    }



    return 0;
}

