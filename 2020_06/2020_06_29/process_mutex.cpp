#include <iostream>
#include <cstring>      //for memset()
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>


//设置互斥锁属性以实现进程间同步

struct mval
{
    int num;
    pthread_mutexattr_t mattr;
    pthread_mutex_t mutex;
};


int main()
{   
    //利用mmap()函数映射一块进程间共享的内存区域(内核中)

    /*  实名映射
        int fd = open("temp", O_RDWR | O_CREAT, 0644);
        ftruncate(fd, sizeof(mval)); //截断为指定长度 - 此结构体长度
        mval* pmval = (mval*)mmap(NULL, sizeof(mval), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        unlink("temp");
        */
    //匿名映射 - 一块sizeof(mval)的内存大小区域用于进程间共享
    mval* pmval = (mval*)mmap(NULL, sizeof(mval), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    memset(pmval, 0, sizeof(mval));

    //初始化num值

    pmval->num = 100;

    //设置互斥锁的属性 - 限于初始化此互斥锁
    pthread_mutexattr_init(&pmval->mattr);
    pthread_mutexattr_setpshared(&pmval->mattr, PTHREAD_PROCESS_SHARED); //进程间共享

    pthread_mutex_init(&pmval->mutex, &pmval->mattr);    //初始化一把互斥锁 - 此锁用于进程间的同步

    pid_t pid = fork();
    if( pid == 0 )  //子线程
    {
        for(int i = 0; i < 10; ++i)
        {

            pthread_mutex_lock(&pmval->mutex); //访问数据前加锁

            pmval->num++;
            std::cout << "---child num++ -> " << pmval->num << std::endl;

            pthread_mutex_unlock(&pmval->mutex);
            sleep(1);
        }
    }
    else if( pid > 0 )  //父线程
    {
        for(int i = 0; i < 10; ++i)
        {

            pthread_mutex_lock(&pmval->mutex); //访问数据前加锁

            pmval->num += 10;
            std::cout << "parent num+10 -- -> " << pmval->num << std::endl;

            pthread_mutex_unlock(&pmval->mutex);
            sleep(1);
        }
    }

    wait(NULL);//回收子线程

    //销毁相关互斥锁属性及互斥锁
    pthread_mutexattr_destroy(&pmval->mattr);
    pthread_mutex_destroy(&pmval->mutex);


    return 0;
}

