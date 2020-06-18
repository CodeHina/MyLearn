#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

void* tfunc(void* arg)
{
    pthread_attr_t attr;
    void* stackaddr = NULL;
    size_t stacksize;

    pthread_attr_init(&attr);
    pthread_attr_getstack(&attr, &stackaddr, &stacksize);
    //int pthread_attr_getstacksize()
    std::cout << "线程的栈大小为 - " << stacksize << "addr : 0x" << std::hex << stackaddr << std::endl;

    pthread_exit((void*)1);
}


int main()
{
    pthread_t tid;
    int count = 0;

    int ret =  pthread_create(&tid, NULL, tfunc, NULL);
    if( 0 != ret )
    {
        std::cerr << "pthread_create : " <<  strerror(ret) << std::endl;
    }

    std::cout << "最多创建" << count << "个线程" << std::endl;
    
    

    pthread_exit((void*)0);
}

