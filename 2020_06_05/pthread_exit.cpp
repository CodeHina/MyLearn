#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

void* thrd_func(void* arg)//子线程主控函数
{
    
    //循环创建子进程时 - arg参数可传递值 - 传递地址会造成不同线程间使用同一块地址
    //需要根据不同需求来确定传递参数

    //sleep((long)arg);
    std::cout << (long)arg <<  " thread - In thrd_func : thrdid is " << pthread_self() << " pid is " << getpid() << std::endl;

    return NULL;
}


int main()
{

    int ithread = 0; //线程个数

    for(ithread = 0; ithread < 5; ithread++)
    {

        pthread_t thrdid;//线程id

        //int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);
        //参数1:线程id 参数2:线程属性(默认属性传NULL) 参数3:线程主控函数 参数4:线程主控函数的参数
        int ret = pthread_create(&thrdid, NULL, thrd_func, (void*)(long)ithread); //成功返回0/失败返回errno
        //传递当前进程的个数 - ithred(从int向void*转换 - 64位下是小转大,高位补零)
        //且不可用(void*)&ithred - 这样每个线程所传递的参数都是同一块内存地址的值 - 会出现错误
        if( ret != 0) 
        {
            //char* strerror(int errnum) / char* strerror_r(int errnum, char* buf, size_t buflen) - 线程安全的
            char buf[128];
            strerror_r(ret, buf, sizeof(buf));
            std::cerr << "pthread_create error : " << buf << std::endl;
            exit(-1);
        }

    }

/*
    //由于调用了pthread_create(),进程已经变成了线程
    //main()函数调用return 0;时会结束整个进程 - 即此进程内存空间会被操作系统回收 - 线程自然结束
    //所以调用sleep()函数保证主控线程不会早于子线程结束

    sleep(ithread); //保证子线程主控函数的调用成功

    return 0;
*/
    
    //可调用void pthread_exit(void*);函数来只终止当前线程
    
    std::cout << "I'm main thread, not a process!" << " - main_thread_id is " << pthread_self() << std::endl;

    pthread_exit(NULL);//以NULL值退出 - 可传其他值

}

