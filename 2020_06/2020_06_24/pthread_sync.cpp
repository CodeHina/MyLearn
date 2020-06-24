#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>

using namespace std;

pthread_mutex_t mutex; //定义互斥锁

void* tfunc(void* arg)
{
    srand( time(NULL) );//线程

    while(1)
    {
        //占用共享资源前对资源进行加锁
        pthread_mutex_lock(&mutex);//相当于对mutex进行--

        cout << "hello";
        sleep( rand() % 3 );    //模拟长时间共享资源 - cpu被其他线程占用
        cout << "\tworld\n";
        //sleep( rand() % 3 );
        
        //不占用后进行解锁 - 被阻塞等待的线程竞争加锁
        pthread_mutex_unlock(&mutex);


        sleep( rand() % 3 );
    }

}

//由于主控线程和子线程共享了同一个资源(stdout - 标准输出) - 导致打印的顺序混乱 - 需要进行加互斥锁进行约束

int main()
{
    pthread_t tid;
    srand( time(NULL) );
    
    //初始化互斥锁
    //int pthread_mutex_lock(const pthread_mutex_t* restrict mutex, const pthread_mutexattr_t* restrict attr);
    pthread_mutex_init(&mutex, NULL);   //mutex == 1

    pthread_create(&tid, NULL, tfunc, NULL);

    while(1)//主控线程
    {
        //每次占用某共享资源时 - 进行加锁 
        //int pthread_mutex_lock(pthread_mutex_t* mutex);
        //成功返回0 - 失败返回错误号 - 利用strerror_r()打印错误号及相关信息
        pthread_mutex_lock(&mutex);//相当于对mutex进行--
        
        cout << "HELLO";
        sleep( rand() % 3 );    //模拟长时间共享资源 - cpu被其他线程占用
        cout << "\tWORLD\n";
        //sleep( rand() % 3 );
    
        pthread_mutex_unlock(&mutex);//相当于对mutex进行解锁

        sleep( rand() % 3 );
    
    }


    pthread_join(tid, NULL);//回收非非分离态的子线程


    pthread_mutex_destroy(&mutex);//销毁互斥锁

    return 0;
}

