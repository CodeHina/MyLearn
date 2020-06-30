#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>


//哲学家吃饭问题 - 多线程版本

const int N = 5;
const int ERRINFO_BYTES = 64;

int shared_num = -1;            //共享数据
pthread_mutex_t chopsticks[N];  //N只筷子 

void* tfunc(void* arg);

void sys_pthrd_err(const char* err_func, int ret)
{
    if(ret != 0)
    {
        char buf[ERRINFO_BYTES];
        char* errinfo = strerror_r(ret, buf, sizeof(buf));
        std::cerr << err_func << " : " << errinfo << std::endl;
        exit(-1);
    }
}

int main()
{
    pthread_t philoer[N];           //N个哲学家

    for(int i = 0; i < N; ++i)
        sys_pthrd_err("pthread_mutex_init",pthread_mutex_init(&chopsticks[i], NULL));


    int i = 0;//创建线程 - 传递线程编号
    for(int i  = 0; i < N; ++i)
        sys_pthrd_err( "pthread_create", 
                       pthread_create(&philoer[i], NULL, tfunc, (void*)i));

    for(int i = 0; i < N; ++i)      //主控线程阻塞等待回收子线程 - 也可设置子线程为分离态使得子线程结束后自动回收
        pthread_join(philoer[i], NULL);

    for(int i = 0; i < N; ++i)
        sys_pthrd_err("pthread_mutex_destroy",pthread_mutex_destroy(&chopsticks[i]));

    pthread_exit((void*)0);
}

void* tfunc(void* arg)
{
    int ithrd = (int)arg; //线程编号

    int left = -1, right = -1; //互斥锁编号

    if(ithrd == (N-1))    //当前线程为最后一个线程 - 改变拿筷子的顺序 - 先去拿右筷子 - 改变left为右筷子
    { left = 0; right = ithrd; }       
    else
    { left = ithrd; right = ithrd + 1; }

    //left = ithrd; right = (ithrd + 1) % N; //这种情况会造成死锁

    while(1)
    {
        sys_pthrd_err("pthread_mutex_lock", pthread_mutex_lock(&chopsticks[left]) );  //加锁左筷子

        //std::cout << ithrd << " get " << left << " left lock---" << std::endl;
        
        sys_pthrd_err("pthread_mutex_lock", pthread_mutex_lock(&chopsticks[right]) ); //加锁右筷子
        
        //std::cout << "***" << ithrd << " get " << right << " right lock" << std::endl;

        shared_num += (ithrd);   //吃饭 - 处理共享数据
    
        std::cout << "in " << ithrd << " shared_num changed as " << shared_num << std::endl;
    

        sys_pthrd_err("pthread_mutex_unlock", pthread_mutex_unlock(&chopsticks[right])); //解锁右筷子
        sys_pthrd_err("pthread_mutex_unlock", pthread_mutex_unlock(&chopsticks[left]));  //解锁左筷子

        usleep(2000);   //保证其他线程抢占到CPU
    }

    pthread_exit(arg);
}
