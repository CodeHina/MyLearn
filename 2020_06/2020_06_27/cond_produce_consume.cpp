#include <iostream>
#include <pthread.h>
#include <unistd.h>


//线程同步 - 条件变量 - 模拟生产者和消费者模型

struct msg
{
  struct msg * next;
  int num;
};//一个信息节点 - 模拟生产出的产品

struct msg* head;
struct msg* p;  


void* produce(void*);//生产者
void* consume(void*);//消费者


pthread_cond_t cond;  //条件变量
pthread_mutex_t mutex;//互斥锁

int main()
{
    pthread_mutex_init(&mutex, NULL); //初始化
    pthread_cond_init(&cond, NULL); 

    pthread_t tpid, tcid;            //生产者、消费者

    pthread_create(&tpid, NULL, produce, NULL);
    pthread_create(&tcid, NULL, consume, NULL);

    pthread_join(tpid, NULL);
    pthread_join(tcid, NULL);

    pthread_exit((void*)0);
}


void* produce(void* arg)//生产者
{
    while(1)
    {
        p = new msg;//produce a node
        p->num = rand() % 100; 

        //将生产的节点放入共享的资源区 - 操作共享区域需加速
        
        pthread_mutex_lock(&mutex);
        p->next = head;             //头插法 - 插入节点 - head总是指向新节点
        head = p;
        pthread_mutex_unlock(&mutex);

        std::cout << "--------produce a node : "<< head->num << " and link to head----------" << std::endl;

        pthread_cond_signal(&cond); //唤醒由此条件阻塞的消费者

        sleep(1);
    }
    return (void*)1;
}

void* consume(void* arg)//消费者
{
    while(1)
    {
        pthread_mutex_lock(&mutex); //访问共享数据时加互斥锁

        while(head == NULL)//当前无产品时
        {
            pthread_cond_wait(&cond, &mutex); //注册条件变量 - 以及解锁mutex(以便生产者访问此共享数据)
        }

        //有产品时就消费产品(注意加锁数据) - 访问节点 - 此时新节点为当前的头节点
        msg* temp = head;
        head = head->next;
        
        pthread_mutex_unlock(&mutex); //访问数据 - 解锁互斥锁

        std::cout << "******consume a node : " << temp->num << " and delete node*********" << std::endl;
        delete temp;

        sleep(3);
    }
    
    return (void*)2;
}
