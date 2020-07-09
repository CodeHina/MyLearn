
/*
   实现多线程的并发服务器

   同样和进程逻辑相同 - 主线程控制等待客户端连接 - 子线程负责处理通信
   由于线程退出同样需要去回收线程资源 - 故线程需设置为分离态 —— 线程退出自动回收资源
   对于套接字 - 线程共享.data\.bbs\heap\数据 - 故需将主线程中的c_socket以及必要的信息封装传递给线程(值传递无需考虑线程同步)
   若涉及对共享资源的访问 - 需考虑线程同步
   系统负载大 - 若连接客户端增多 - 导致有的线程不能及时获取CPU处理数据

*/

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

#include "../include/Server.h"
#include "../include/sys_err.h"


const int MAX_THRD_NUM = 256;

struct thrd_info
{
    int c_socket;
};


void* thrd_communicate(void* arg);

void Handle(int);

int main()
{
    pthread_t tid = -1;

    thrd_info* tinfo = new thrd_info[MAX_THRD_NUM]; //最大同时工作线程数量
    int ithrd = 0;  //当前线程表示符

    Server s;

    s.SetConfig();

    while(1)
    {
        s.AcceptClient();
        tinfo[ithrd].c_socket = s.get_c_socket();

        int ret = pthread_create(&tid, NULL, thrd_communicate, (void*)&tinfo[ithrd] );
        if(ret != 0)
        {
            char buf[64];
            std::cerr << "pthread_create err" << strerror_r(ret, buf, sizeof(buf));
            exit(-1);
        }
        ++ithrd;
    }

    delete [] tinfo;

    return 0;
}


void* thrd_communicate(void* arg)
{
    thrd_info* tinfo = (thrd_info*)arg;

    Handle(tinfo->c_socket);

    close(tinfo->c_socket);

    int ret = pthread_detach(pthread_self());
    if(ret != 0)
    {
        char buf[64];
        std::cerr << "pthread_detach err" << strerror_r(ret, buf, sizeof(buf));
        exit(-1);
    }

    usleep(200);
}


