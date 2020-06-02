/*
 
   ---------------------守护进程模型的建立---------------------------
    
    1.创建子进程 - 父进程退出
      
        守护进程只能利用子进程创建,进程组组长无法调用setsid()函数

    2.子进程中调用setsid()创建会话

        创建会话 - 使得子进程独立 - 成为新的进程组组长
        The calling process will be the only process in the new process group and in the new session.

    3.改变当前的工作目录 - 一般为根目录
        
        防止占用可卸载文件系统 - 若可执行文件所在工作目录可卸载,若中途卸载导致文件无法正确执行

    4.重设文件权限掩码

        防止创建的文件的屏蔽字拒绝某些权限

    5.关闭文件描述符
        
        由于新建会话的特性是退出当前终端,则继承的文件不会用到,不做处理的话浪费系统资源
        可将STDIN_FILENO\STDOUT_FILENO\STDERR_FILENO重定向到/dev/null
    
    6.执行守护进程的核心工作 - 主逻辑
    
 */

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

inline void Perror(const char* s)
{
    perror(s);
    exit(-1);
}


int main()
{
        
    //1.创建子进程
    pid_t cpid = fork();
    if(cpid == -1)
        Perror("fork");

    //2.子进程创建会话
    if(cpid == 0)
    {
        if( setsid() == -1 )
            Perror("setsid");
            
        //3.改变当前的工作目录
        if( chdir("/home/exhina") == -1)
            Perror("chdir");

        //4.设置umask掩码 - 函数调用默认成功 - 创建文件权限为664
        umask(0002);           
        
        //5.重定向0/1/2文件描述符
        close(STDIN_FILENO);
        int fd = open("/dev/null", O_RDWR); //fd == 0
        if(fd == -1)
            Perror("open");
        if( dup2(0, STDOUT_FILENO) == -1)
            Perror("dup2 stdout");
        if( dup2(0, STDERR_FILENO) == -1)
            Perror("dup2 stderr");    

        //return 1;
    }
    else
        return 0; //父进程直接退出
    
    //6.守护进程主逻辑 - 不断向一个文件写数据
    
    int tempfd = open("/home/exhina/MyLearn/Temp/daemon.txt", O_RDWR | O_CREAT, 0644);
    if(tempfd == -1)
        Perror("open temp.txt");

    if( truncate("/home/exhina/MyLearn/Temp/daemon.txt", 0) == -1 )
        Perror("truncate");

    const char* str = "a daemon is working\n";

//注意先屏蔽信号
    
    //注册一个信号捕捉函数 - 当有进程向当前此守护进程发送指定信号时即执行某种动作
        //某种动作 - 如关闭当前打开的文件 - 释放缓冲区 -占用的锁等资源释放 - 然后退出或者杀死自己  



//再解除信号 - 保证注册时不会处理该信号 - 时序安全

    while(1)
    {
        write(tempfd, str, strlen(str));
        sleep(3);
    }

    return 0;
}

