#include <iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <error.h>


typedef struct //线程退出信息
{
    unsigned long exit_id;
    char exit_msg[64];
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

    pthread_t tid;
    pthread_exit_t* retval;
    pthread_exit_t* emsg = new pthread_exit_t;
    
    int ret = pthread_create(&tid, NULL, tfunc, (void*)emsg);
    if(0 != ret)
    {
        pthrd_err(ret);
    }


    sleep(2);

    std::cout << "In main_thread  - add_of_emsg : " << std::hex << emsg << std::endl;

    //调用pthread_jojn()回收进程并获取进程退出状态
    //会阻塞等待子线程退出 - 子线程已经结束则立即返回
    //int pthread(pthread_t pthread, (void**)retval);  - 参数2为退出值的指针
    int ret1 =  pthread_join(tid, (void**)&retval);
    if(0 != ret1)
    {
        pthrd_err(ret1);
    }
    
    std::cout << std::dec << retval->exit_id << "\n" << retval->exit_msg;
    
    delete emsg;
    //delete (pthread_exit_t*)emsg; //释放在线程中分配的堆内存 - 会出现段错误 - 为什么 - 
    //这是由于在主控线程中只是传递了指针的值,而在子线程中又由于分配堆内存改变了这个值
    //所以采用在主控线程中分配内存,创建线程时传递堆内存地址 - 最后等待回收完子线程后在主控线程中释放堆内存
    //主控线程退出
    pthread_exit((void*)0);

}


void* tfunc(void* arg)
{
    std::cout << "tid = " << pthread_self()  << "\tpid = " << getpid() << std::endl;
    
    pthread_exit_t * emsg = (pthread_exit_t*)arg;
    std::cout << "In child_thread - add_of_emsg : " << std::hex << emsg << std::endl;
    //emsg = new pthread_exit_t;
    

    emsg->exit_id = pthread_self();
    strcpy(emsg->exit_msg, "thread_exit\n");

    pthread_exit((void*)emsg);  //子线程以emsg值退出

}


