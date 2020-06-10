#include <iostream>
#include <pthread.h>
#include <unistd.h>

void* tfunc1(void* arg)
{
    std::cout << "-----thread 1-----" << std::endl;
    return (void*)1;
}

void* tfunc2(void* arg)
{
    std::cout << "-----thread 2-----" << std::endl;
    pthread_exit( (void*)2 );
}

void* tfunc3(void* arg)
{
    while(1)
    {
        //sleep(1);   //系统调用的取消点 - man 7 pthreads查看取消点的系统调用
        //std::cout << "-----thread 3-----" << std::endl; //也会调用系统I/O的系统调用
        
        //若无此类取消点 - 需自行加入
        pthread_testcancel();//定义自己的取消点
    }
    return (void*)3;
}


int main()
{
    pthread_t tid1, tid2, tid3;
    void* termval = NULL;

    pthread_create(&tid1, NULL, tfunc1, NULL);
    pthread_join(tid1, &termval);
    std::cout << tid1 << " end with " << (int)termval << std::endl;

    pthread_create(&tid2, NULL, tfunc2, NULL);
    pthread_join(tid2, &termval);
    std::cout << tid2 << " end with " << (int)termval << std::endl;

    pthread_create(&tid3, NULL, tfunc3, NULL);
    sleep(1);
    pthread_cancel(tid3);
    pthread_join(tid3, &termval);
    std::cout << tid3 << " end with " << (int)termval << std::endl;


    return 0;
}

