#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>

void pthrd_err(int ret)
{
    char buf[128];
    char* err_str =  strerror_r(ret, buf, sizeof(buf));

    std::cerr << "pthread_join err : " <<  err_str << std::endl;
}


void* tid_func(void* arg)
{
    //pthread_cond_timedwait() - 让进程等待一段时间

    std::cout << "thread doing something" << std::endl;

    pthread_exit((void*)1);
}


int main()
{
    
    pthread_t tid;

    //在创建前设置线程属性以实现分离进程
    //函数 - pthread_attr_init(pthread_attr_t* attr); - pthread_attr_setdetach(pthread_attr_t*attr, int detachstate);
    //其属性初始化与设置都在创建线程之前
    
    pthread_attr_t attr;
    int errno1 = pthread_attr_init(&attr);
    if(0 != errno1)
        pthrd_err(errno1);

    int errno2 = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置线程分离
    if(0 != errno2)
        pthrd_err(errno2);
    
    int dstate = -1;
    int errno3 = pthread_attr_getdetachstate(&attr, &dstate);
    if(0 != errno3)
        pthrd_err(errno3);
    if( PTHREAD_CREATE_DETACHED == dstate )
    {
        std::cout << "线程分离态设置完成 - " << dstate << std::endl;
    }



    //创建一个线程
    int ret = pthread_create(&tid, &attr, tid_func, NULL);
    if(0 != ret)
    {
        pthrd_err(ret);
    }
/*
    //将此线程设置为detach - 该线程一旦结束就立即回收所占用的系统资源
    int ret2 = pthread_detach(tid);
    if(0 != ret2)
    {
        pthrd_err(ret2);
    }
*/  
    //若此时还使用pthread_join()回收该线程 - 会错误返回并设置errno为EINVAL
    int ret3 = pthread_join(tid, NULL);
    if(0 != ret3 && EINVAL == ret3)
    {
        //std::cout < "捕捉到EINVAL" << std::endl;
        std::cerr << "errno : " << ret3 << std::endl;
        pthrd_err(ret3);
    }


    pthread_exit((void*)0);
}

