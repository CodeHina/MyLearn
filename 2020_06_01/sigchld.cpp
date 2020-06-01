//利用SIGCHLD信号来回收子进程
//SIGCHLD信号产生条件: 1.终止进程 2.接收SIGSTOP信号暂停进程 3.暂停态接收SIGCONT信号
//子进程执行exec()类函数时成功不会返回


void do_sig_child(int);

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t cpid = -1;
    int ichld = -1;

    for(ichld = 0; ichld < 5; ++ichld)          //循环创建5个子进程
    {
        if( ( cpid = fork() ) == -1)
            perror("fork");
        else if(cpid == 0)
            break;
    }

    if(cpid == 0) //当前进程为子进程
    {
        std::cout << "child " << ichld + 1 << " pid : " <<  getpid() << std::endl;
        sleep(3);
        return ichld + 1;
    }
    else if(cpid > 0) //当前进程为父进程
    {

        struct sigaction act, old_act;

        act.sa_handler = do_sig_child;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);

        sigaction(SIGCHLD, &act, &old_act); //注册信号捕捉函数

        while(1)
        {
            std::cout << "ppid : " << getpid() << std::endl;
            sleep(2);
        }


    }

    return 0;
}

void do_sig_child(int sig)
{
    //利用pid_t waitpid(pid_t pid, int* wstatus/*O*/, int options);
    //pid - 0(回收任意子进程,且子进程组pid与当前调用进程的进程组pid相同)
    //wstatus - 保存回收的子进程的退出信息
    //options - 回收状态 - WNOHANG(非阻塞形式等待子进程 - 无子进程结束则立即返回)
    pid_t ret_pid = -1;
    int wstatus = -1;


    //这里利用if会出错 - 造成不能完全回收所有进程 - 产生僵尸进程
    //Linux内核通过判断未决信号集来进行信号处理
    //且在信号捕捉期间 - 有多个信号发送给进程 - 进程只记录一次(处理一次)
    //即可能在此函数执行期间 - 有新的子进程几乎同时终止并给父进程发送SIGCHLD信号
    //但是父进程只记录一次 - 就产生了僵尸进程
    //所以将if改为while - 轮询的去回收僵尸进程
    //例-同时2/3/4三个进程死亡 - 处理信号时先回收了2号进程 - 由于while存在又回收了已经死亡的3/4号进程
    //但是信号捕捉函数只执行了一次
    
    std::cout << "----do_sig_child()-----" << std::endl;

    while( (ret_pid = waitpid(0, &wstatus, WNOHANG)) > 0) //返回值为0/-1(无子进程返回/执行错误)
    {
        if( WIFEXITED(wstatus) )                //进程正常终止
            std::cout << "child " << getpid() << " exit with " << WEXITSTATUS(wstatus) << std::endl;//打印其退出值
        else if( WIFSIGNALED(wstatus) )         //进程被信号终止
            std::cout << "child " << getpid() << " exit with " << WTERMSIG(wstatus) << std::endl;   //打印此信号
    }
}
