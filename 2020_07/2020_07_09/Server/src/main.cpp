
/*
   实现多进程的高并发服务器
   1.基本实现
   当有多个客户端对服务器进行访问的时候,A客户端和服务器建立连接后,服务端accept()后产生一个c_socket和A进行通信
   当再有一个客户端想和服务器建立连接时,服务器正在和A进行通信,故无法实现
   故考虑创建多个进程来分担服务器和客户端通信

   服务端父进程只关心等待客户端建立连接,每建立一个连接便创建子进程和当前客户端进行通信

   特别地,父子进程是共享文件描述符(共享打开的c_socket)
   由于父进程只进行等待,可将c_socket关闭
   子进程只进行通信,可将my_socket关闭

   2.当子进程接受完客户端数据,客户端四次挥手和服务器端释放连接后,子进程read()函数返回0 - 子进程自然退出

   linux操作系统子进程退出后会产生僵尸进程,需及时回收僵尸进程释放资源
   但是父进程在等待其他客户端连接 - 无法退出主逻辑进行有效回收
   根据信号的机制可知 - 子进程终止时会向父进程发生SIGCHLD信号
   故考虑注册信号捕捉函数进行回收终止的子进程
   */

#include "../include/Server.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void wait_child(int sign)
{
    int wstatus;
    while(waitpid(-1, &wstatus, WNOHANG) > 0)//不阻塞回收子进程
    {
        if( WIFEXITED(wstatus) != 0)
        {
            //WEXITSTATUS(wstatus) - 子进程正常终止返回值
            return;
        }
        else if( WIFSIGNALED(wstatus) )
        {
            //WTERMSIG(wstatus) - 子进程被此信号终止
            return;
        }
    }
}



int main()
{
    Server s;

    s.SetConfig();

    pid_t pid = -1;
    while(1)
    {
        s.AcceptClient();

        pid = fork();
        if(pid == -1)
        {
            perror("fork err");
            exit(-1);
        }
        else if(pid == 0)//子进程跳出当前循环
            break;
        else//父线程 - 注册信号处理函数
        {
            close(s.get_c_socket());    //关闭通信的套接字
            signal(SIGCHLD, wait_child);
        }
    }

    if(pid == 0)//子线程通信逻辑
    {
        close(s.get_my_socket());       //关闭等待连接的套接字
        s.Handle();
    }



    return 0;
}
