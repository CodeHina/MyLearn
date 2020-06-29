#include <iostream>
#include <semaphore.h> //信号量相关函数头文件
#include <unistd.h>

//利用信号量模拟生产者消费者模型 - 共享数据区采用一个循环队列
//维护两个信号量 - sem_blank \ sem_product - 共享区空格数(最大允许访问的线程数) \  产品数(生产者生产的数量)
//生产者生产到满足空格数后即阻塞
//消费者消费至产品数为0即阻塞
//sem_wait() - 相当于对信号量加速 - 即信号量--  - 信号量为0则阻塞
//sem_post() - 相当于对信号量解锁 - 即信号量++  - 信号量为N时则阻塞  - 且此时唤醒阻塞在信号量的线程

const int N = 5;              //最大允许共享数据的线程数
sem_t sem_blank, sem_product; //空格数 - 产品数
int queue[N] = {0};           //循环队列


void* produce(void*);   //生产者主控函数
void* consume(void*);   //消费者主控函数

void sem_err(int ret)
{
    if(ret == -1)
        perror("sem_func err");
}

int main()
{
    pthread_t tpid, tcid; //生产者线程id\消费者线程id

    sem_init(&sem_blank, 0, N);     //初始化空格数信号量为N
    sem_init(&sem_product, 0, 0);   //初始化产品数信号量为0

    pthread_create(&tpid, NULL, produce, NULL); //分别创建生产者和消费者线程
    pthread_create(&tcid, NULL, consume, NULL);

    pthread_join(tpid, NULL);
    pthread_join(tcid, NULL);

    sem_destroy(&sem_blank);        //销毁两个信号量
    sem_destroy(&sem_product);

    pthread_exit( (void*)0 );
}

void* produce(void* arg)   //生产者主控函数
{
    int i = 0;
    while(1)
    {
        sem_err( sem_wait(&sem_blank) ); //对访问的共享数据进行加锁 - 即对空格数进行填充 - 空格数-1 - 当前同时访问数据线程数-1

        //模拟生产
        queue[i] = rand() % 10 + 1; //生产一个1——10之间的数字
        std::cout << "---produnce a num : " << queue[i] << " ---" << std::endl; 

        sem_err( sem_post(&sem_product) ); //对访问的共享数据进行解锁 - 并唤醒阻塞的线程 - 即产品数+1 - 当前可同时访问数据线程数+1

        i = (i + 1) % N; //循环队列重新定址 - 已入队一个产品

        sleep(1);

    }

    pthread_exit( (void*)1 );

}

void* consume(void* arg)  //消费者主控函数
{
    int i = 0;
    while(1)
    {
        sem_err( sem_wait(&sem_product) ); //对访问的共享数据进行加锁 - 即产品数-1
        
        //模拟消费
        std::cout << "###consume a num : " << queue[i] << " ###" << std::endl;
        queue[i] = -1; //当前消费节点置为-1
    
        sem_err( sem_post(&sem_blank) );  //消费结束 - 空格数+1 - 若生产者线程阻塞则唤醒生产者进行生产 - 空格数+1

        i = (i + 1) % N;
    
        sleep(3);
    }

    pthread_exit( (void*)2 );
}

