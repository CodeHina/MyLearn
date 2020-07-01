#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <cstring>

const int N = 5;
const int ERRINFO_BYTES = 64;


void sys_process_err(const char* err_func, int ret)
{
    if(ret == -1)
    {
        perror(err_func);
        exit(-1);
    }
}

struct shared_msg //共享数据 - 进程间只能通过共享映射区等方法实现进程间共享数据
{
    sem_t chopsticks[N];
    int shared_num;
};

int main()
{

    //sem_t *chopsticks;       //N个信号量 - 这里特别注意进程间共享数据(进程间通信) - 需要依靠内核映射区 
    //也可采用改变互斥锁属性来用于进程间同步

    //采用mmap匿名映射的一块sem_t[N]大小的区域存放信号量 - 并初始化映射区 
    shared_msg* pmsg = (shared_msg*)mmap(NULL, sizeof(shared_msg), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    memset(pmsg, 0, sizeof(shared_msg));
    pmsg->shared_num = -1;

    for(int i = 0; i < N; ++i)//pshared - 1非0表示进程间
        sem_init(&pmsg->chopsticks[i], 1, 1);//规定同一时刻只有一个进程可访问 - 即互斥锁


    int i = 0;//创建5个子进程
    for(i = 0; i < N; ++i)
    {
        pid_t pid = fork();
        sys_process_err("fork", pid);
        if(pid == 0)
            break;
    }

    if(0 <= i && i < N)//子进程主逻辑
    {
        int left = -1, right = -1;

        if(i == (N - 1))                 //有一个人逆着去取筷子(加锁)
        { left = 0; right = N - 1; }
        else
        { left = i; right = i + 1; }

        //left = i; right = (i + 1) % N; //这种情况会出现死锁

        while(1)
        {
            sem_wait(&pmsg->chopsticks[left]);
            
            //std::cout << i << "get " << left << " left lock" << std::endl;

            sem_wait(&pmsg->chopsticks[right]);
            
            pmsg->shared_num += i;

            std::cout << "in " << i << " shared_num changed as " << pmsg->shared_num << std::endl;

            sem_post(&pmsg->chopsticks[right]);
            sem_post(&pmsg->chopsticks[left]);

            usleep(200);
        }

    }
    else if(i == N)     //父进程 - 回收子进程 - 销毁信号量
    {
        for(int i = 0; i < N; ++i)
            wait(NULL);

        for(int i = 0; i < N; ++i)//销毁信号量
            sem_destroy(&pmsg->chopsticks[i]);

        return 0;
    }


}

