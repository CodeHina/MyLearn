#include <iostream>
#include <unistd.h>
#include <pthread.h>


class Signleton
{
private:

    Signleton() 
    { 
        std::cout << "当前创建了" << (m_count++) << "对象\n";
    }

public:

    static Signleton* single;
    static Signleton* get_single_inst();
    static int m_count;
    static pthread_mutex_t mutex;

};

Signleton* Signleton::single = nullptr;

int Signleton::m_count = 0;

pthread_mutex_t Signleton::mutex = PTHREAD_MUTEX_INITIALIZER;//静态初始化互斥锁

Signleton* Signleton::get_single_inst()
{
    pthread_mutex_lock(&mutex);

    usleep(2000);       //确保其他线程能够获得cpu

    if( m_count < 1 && single == nullptr )
        single = new Signleton;//调用构造函数

    pthread_mutex_unlock(&mutex);

    return single;
}

void* tfunc(void* arg)
{
    std::cout << "pthread_id : " << pthread_self() << "\n";

    Signleton::get_single_inst();   //在所有线程共享的堆内存上创建一个类对象 - 单例模式要求只有一个类对象
    
    pthread_exit((void*)1);
}

/* 单例模式-懒汉式 - 线程安全版
 * 不安全原因：
 *      在调用此类的静态函数即创建一个对象 - 需要时才调用
 *      在多线程情况下当1个线程运行到if(m_count < 1 && single == nullptr)失去CPU,符合条件准备创建对象
 *      但是2线程获得CPU后也顺利创建了对象 - 1线程并不知道2线程创建了,则又创建了对象 - 破坏了单例模式
 *      故线程不安全
 *
 * 上述问题是由于在判断阶段失去了cpu - 考虑在执行此判断并创建实例时(即访问共享资源时)进行加互斥锁
 * 保证当前线程访问完single变量才让其他线程访问此变量以此保证线程安全
 * */


int main()
{
    const int ithrd = 10;
    pthread_t tid[ithrd];

    for(int i = 0; i < ithrd; i++)
        pthread_create(&tid[i], NULL, tfunc, NULL);

    for(int i = 0; i < ithrd; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_exit(0);


}

