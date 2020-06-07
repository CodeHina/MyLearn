#include <iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <error.h>

const int N_THREAD = 5;

typedef struct //线程退出信息
{
    int  exit_id;       //线程计数id
    char exit_msg[64];  //退出返回的字符串信息
}pthread_exit_t;


void* tfunc(void*);

void pthrd_err(int ret)
{
    char buf[128];
    strerror_r(ret, buf, sizeof(buf));

    std::cerr << buf << std::endl;
    exit(-1);
}


int main()
{

    pthread_t tid[N_THREAD];                                //存放5个子线程的线程id
    pthread_exit_t* retval[N_THREAD];                       //存放5个线程的返回值 - 供pthread_join()函数使用
    pthread_exit_t* emsg = new pthread_exit_t[N_THREAD];    //分配一个堆内存数组 - 存放退出信息
    
    int ithrd;//线程计数器
    for(ithrd = 0; ithrd < N_THREAD; ++ithrd)
    {
        emsg[ithrd].exit_id = ithrd;
        int ret = pthread_create(&tid[ithrd], NULL, tfunc, (void*)(emsg + ithrd));
        if(0 != ret)
        {
            pthrd_err(ret);
        }
    }


    std::cout << "In main_thread  - add_of_emsg : " << std::hex << emsg << std::endl;

    //调用pthread_jojn()回收进程并获取进程退出状态
    //会阻塞等待子线程退出 - 子线程已经结束则立即返回
    //int pthread(pthread_t pthread, (void**)retval);  - 参数2为退出值的指针
    for(int i = 0; i < N_THREAD; ++i)
    {
        int ret1 =  pthread_join(tid[i], (void**)&retval[i]);
        if(0 != ret1)
        {
            pthrd_err(ret1);
        }
    }

    for(int i = 0; i < N_THREAD; ++i)
        std::cout << std::dec << retval[i]->exit_id << "\n" << retval[i]->exit_msg;


    //delete (pthread_exit_t*)emsg; //释放在线程中分配的堆内存 - 会出现段错误 - 为什么 - 
    //这是由于在主控线程中只是传递了指针的值,而在子线程中又由于分配堆内存改变了这个值
    //所以采用在主控线程中分配内存,创建线程时传递堆内存地址 - 最后等待回收完子线程后在主控线程中释放堆内存
    delete []emsg;
    
    std::cout << pthread_self() << " - main_thread_exit" << std::endl;

    //主控线程退出
    pthread_exit((void*)0);

}


void* tfunc(void* arg)
{
    
    std::cout << "--------------------------------" << std::endl;

    std::cout << std::dec 
              <<  "tid = " << pthread_self()  
              << "\tpid = " << getpid() << std::endl;
    

    pthread_exit_t * emsg = (pthread_exit_t*)arg;
    
    sprintf(emsg->exit_msg, "%lu - thread_exit\n", pthread_self());

    std::cout << "In " << emsg->exit_id  
              << " child_thread - add_of_emsg : " << std::hex << emsg 
              << std::endl;
    
    std::cout << "--------------------------------" << std::endl;


    pthread_exit((void*)emsg);  //子线程以emsg值退出

}


