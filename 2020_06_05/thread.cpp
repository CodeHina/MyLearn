#include <iostream>
#include <unistd.h>
#include <pthread.h>


void* thrd_func(void* arg)//子线程主控函数
{
    
    std::cout << "In thrd_func : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;
    
    return NULL;
}


int main()
{

    std::cout << "In main before create : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;

    pthread_t thrdid;//线程id

    if( pthread_create(&thrdid, NULL, thrd_func, NULL) != 0) //成功返回0/失败返回errno
    {
        std::cerr << "pthread_create error" << std::endl;
        exit(-1);
    }

    std::cout << "In main after  create : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;

    sleep(1); //保证子线程主控函数的调用成功

    return 0;
}

