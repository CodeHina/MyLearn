#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

//利用sem_timedwait()函数 - 一个线程实现读用户输入 - 另外一个线程每隔一段时间向屏幕输出某个信息 
//而一旦用户有输入则立即打印信息
//结合信号量的生产者和消费者模型

void* read(void*);      //两个线程 - 相当于生产者等待产品
void* print(void*);     //相当于消费者消费产品

int flag = -1;          //共享数据

sem_t sem_blank, sem_product;

int main()
{
    pthread_t tpid, tcid;

    sem_init(&sem_blank, 0, 1);
    sem_init(&sem_product, 0, 0);

    pthread_create(&tpid, NULL, read, NULL);
    pthread_create(&tcid, NULL, print, NULL);

    pthread_join(tpid, NULL);
    pthread_join(tcid, NULL);

    sem_destroy(&sem_blank);
    sem_destroy(&sem_product);

    pthread_exit( (void*)0 );
}


void* read(void* arg)
{
    while(1)
    {
        sem_wait(&sem_blank); //加锁共享数据(空格数-1 - 产品等待就位) - 等待用户输入

        //std::cout << "please input : \n";
        std::cin >> flag;

        if(flag != -1)              //当前有用户输入 - flag值已经改变
            sem_post(&sem_product); //解锁共享数据(产品数+1) - 并唤醒打印线程
    }

}

void* print(void* arg)
{

    const char* str = "hello,world";

    while(1)
    {
        struct timespec abstime;
        time_t cur = time(NULL); //获取当前时间 - 相对时间
        time_t delta = 10;
        abstime.tv_sec = cur + delta;
        abstime.tv_nsec = 0;

        sem_timedwait(&sem_product, &abstime); //阻塞 delta s 或者有用户输入时立即返回

        std::cout << str << std::endl;         //模拟消费

        flag  = -1;

        sem_post(&sem_blank);               //唤醒读线程(生产者线程)             

        sleep(2);
    }

}








