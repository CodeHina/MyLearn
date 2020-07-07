#include <iostream>
#include <unistd.h>
#include <pthread.h>


class Signleton
{
private:

    Signleton() 
    { 
        sleep(1);
        std::cout << "当前创建了" << (m_count++) << "对象\n";
    }

public:

    static Signleton* single;
    static Signleton* get_single_inst();
    static int m_count;

};

int Signleton::m_count = 0;
Signleton* Signleton::single = new Signleton; //类定义时就已经用静态变量示例化

Signleton* Signleton::get_single_inst()
{
    /*
    if( m_count < 1 && single == nullptr )
        single = new Signleton;//调用构造函数
    */

    return single;                            //再如何多线程 - 都是返回的此静态实例
}

void* tfunc(void* arg)
{
    std::cout << "pthread_id : " << pthread_self() << "\n";

    Signleton::get_single_inst();   //在所有线程共享的堆内存上创建一个类对象 - 单例模式要求只有一个类对象
    
    pthread_exit((void*)1);
}

/* 单例模式 - 饿汉式：
 * 类定义时即初始化了一个实例对象
 * 但是这样每次加载此类就实例对象的做法 - 导致即使不使用此类对象也会占用内存资源 - 造成浪费
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

