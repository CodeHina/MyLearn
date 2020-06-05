#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

void* thrd_func(void* arg)//子线程主控函数
{
    
    std::cout << "In thrd_func : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;
    
    return NULL;
}


int main()
{

    std::cout << "In main before create : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;

    pthread_t thrdid;//线程id

    //int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);
    //参数1:线程id 参数2:线程属性(默认属性传NULL) 参数3:线程主控函数 参数4:线程主控函数的参数
    int ret = pthread_create(&thrdid, NULL, thrd_func, NULL); //成功返回0/失败返回errno
    if( ret != 0) 
    {
        //char* strerror(int errnum) / char* strerror_r(int errnum, char* buf, size_t buflen) - 线程安全的
        char buf[128];
        strerror_r(ret, buf, sizeof(buf));
        std::cerr << "pthread_create error : " << buf << std::endl;
        exit(-1);
    }

    std::cout << "In main after  create : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;

    sleep(1); //保证子线程主控函数的调用成功

    return 0;
}

