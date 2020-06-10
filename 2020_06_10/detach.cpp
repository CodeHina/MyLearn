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
    std::cout << "thread doing something" << std::endl;

    pthread_exit((void*)1);
}


int main()
{
    
    pthread_t tid;

    //创建一个线程
    int ret = pthread_create(&tid, NULL, tid_func, NULL);
    if(0 != ret)
    {
        pthrd_err(ret);
    }

    //将此线程设置为detach - 该线程一旦结束就立即回收所占用的系统资源
    int ret2 = pthread_detach(tid);
    if(0 != ret2)
    {
        pthrd_err(ret2);
    }
    
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

