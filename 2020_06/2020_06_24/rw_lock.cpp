#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

//线程回调函数
void* t_wr(void* arg);  //写操作
void* t_rd(void* arg);  //读操作

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
        //dosomething
    }

}

//全局共享变量

int gg = 99;


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

pthread_rwlock_t rw_lock;//一把全局共享的读写锁

int main()
{
    pthread_t tid[5];

    
    //初始化锁
    sys_err( pthread_rwlock_init(&rw_lock, NULL) );

    //创建5个线程 - 3读 - 2写
    int i = 0; //线程编号

    for(i = 0; i < 3; ++i)
    {
        sys_err( pthread_create(&tid[i], NULL, t_rd, (void*)i));
    }//assert i == 3
    
    std::cout << "读线程创建完毕" << std::endl;

    for(; i < 5; ++i)
    {
        sys_err( pthread_create(&tid[i], NULL, t_wr, (void*)i));
    }//assert i == 5

    std::cout << "写线程创建完毕" << std::endl;


    //回收子线程
    for(int i = 0; i < 5; i++)
        sys_err( pthread_join(tid[i], NULL) );


    //释放资源
    pthread_rwlock_destroy(&rw_lock);

    pthread_exit((void*)0);
}

//读写锁 - 读共享,写独占,写锁优先级高

void* t_rd(void* arg)
{
    int i = (int)arg;

    while(1)
    {   
        //读前加锁
        sys_err( pthread_rwlock_rdlock(&rw_lock) );
        
        std::cout << "-------------read---------- " << i << " gg  = " << gg << std::endl;

        //读后解锁
        sys_err( pthread_rwlock_unlock(&rw_lock) );
        
        sleep(3); //挂起3s - 以便其他线程抢占CPU
    }


    return (void*)arg;

}

void* t_wr(void* arg)
{
    int i = (int)arg;


    while(1)
    {   
        //写前加锁
        sys_err( pthread_rwlock_wrlock(&rw_lock) );
        
        gg -= 1;
        std::cout << "*****************write************ " << i <<  " gg is"  << gg << std::endl;
        
        //写后解锁
        sys_err( pthread_rwlock_unlock(&rw_lock) );
        
        sleep(3); //挂起3s - 以便其他线程抢占CPU
    }


    return (void*)arg;

}

