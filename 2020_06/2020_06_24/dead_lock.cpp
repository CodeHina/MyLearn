#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

//线程回调函数
void* tfunc1(void* arg);
void* tfunc2(void* arg);

//返回值出错判断函数
typedef  int pthread_ret_t;
void sys_err(pthread_ret_t ret)
{
    if(ret != 0)
    {
        char errbuf[32];
        
        char* errstr = strerror_r(ret, errbuf, sizeof(errbuf));
        
        std::cerr << errstr << std::endl;
        
        return;
    }
    else
    {
        std::cout << "pthread_func success" << std::endl;
    }

}

//全局共享变量

char a = 'a';
char b = 'b';



/*
* 死锁现象 
*       1 - 同一个线程对同一个文件锁pthread_mutex_lock()两次 
*           第二次进行lock时上次的锁还未解锁 - 故造成线程的阻塞等待 - 死锁
*           
*       2 - 两个线程 - 线程1拥有锁A,想获取锁B  -  线程2拥有锁B,想获取锁A
*           
*           都想获取对方时 - 还未解锁自己所有的锁 - 故都会造成当前线程阻塞 - 也不会解锁自己的锁 - 两个线程都阻塞
*           解决方法 - 利用trylock()函数来获取锁 - 失败后不会阻塞当前线程
*                      当不能获取所有的锁时 - 就放弃已有的锁
*
*/  

pthread_mutex_t mutex[2];

int main()
{
    pthread_t tid[2];

    //pthread_mutex_t* mutex = new pthread_mutex_t[2]; //分配两个锁
    
    //初始化锁 - mutex[i] == 1
    sys_err( pthread_mutex_init(&mutex[0], NULL) );

    sys_err( pthread_mutex_init(&mutex[1], NULL) );


    //创建两个线程 - 非分离态 - 并将两个锁的地址都传递给线程的回调函数
    pthread_ret_t ret1 = pthread_create(&tid[0], NULL, tfunc1, (void*)(&mutex));
    sys_err(ret1);
    pthread_ret_t ret2 = pthread_create(&tid[1], NULL, tfunc2, (void*)(&mutex));
    sys_err(ret2);
    

    sleep(2);
    std::cout << "wait thread" << std::endl;

    //回收子线程
    std::cout << "开始回收线程1" << std::endl;
    sys_err( pthread_join(tid[0], NULL) );
    std::cout << "结束回收线程1" << std::endl;

    sys_err( pthread_join(tid[1], NULL) );

    //释放资源
    pthread_mutex_destroy(&mutex[0]);
    pthread_mutex_destroy(&mutex[1]);
    //delete []mutex;

    return 0;
}



void* tfunc1(void* arg)
{
    pthread_mutex_t* mutex =  (pthread_mutex_t*)arg ;
    
    pthread_mutex_lock(&mutex[0]);//加锁

    a = 'A';                   //占用资源a
    
    sleep(4);//保证线程2获取锁B
    std::cout << a << " in tfunc1 : then lock B\n";
    pthread_mutex_lock(&mutex[1]); //尝试获取锁B

    pthread_mutex_unlock(&mutex[0]);//解锁

    return (void*)1;

}

void* tfunc2(void* arg)
{

    
    pthread_mutex_t* mutex =  (pthread_mutex_t*)arg ;
    
    pthread_mutex_lock(&mutex[1]);//加锁


    b = 'B';                   //占用资源b
    
    std::cout << b << " in tfunc2 : then lock A\n";
    pthread_mutex_lock(&mutex[0]); //尝试获取锁A

    pthread_mutex_unlock(&mutex[1]);//解锁
    
    return (void*)2;
}
