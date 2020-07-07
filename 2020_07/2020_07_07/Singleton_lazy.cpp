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

};

int Signleton::m_count = 0;
Signleton* Signleton::single = nullptr;

Signleton* Signleton::get_single_inst()
{
    if( m_count < 1 && single == nullptr )
        single = new Signleton;//调用构造函数

    return single;
}

void* tfunc(void* arg)
{
    std::cout << "pthread_id : " << pthread_self() << "\n";

    Signleton::get_single_inst();   //在所有线程共享的堆内存上创建一个类对象 - 单例模式要求只有一个类对象
    
    pthread_exit((void*)1);
}

/* 单例模式-懒汉式：
 *      在调用此类的静态函数即创建一个对象
 *      在多线程情况下当1个线程运行到if(m_count < 1 && single == nullptr)失去CPU,符合条件准备创建对象
 *      但是2线程获得CPU后也顺利创建了对象 - 1线程并不知道2线程创建了,则又创建了对象 - 破坏了单例模式
 *      故线程不安全
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

