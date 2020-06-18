#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
    pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork();


    if(pid == 0)
    {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO); //子进程向管道读数据 - 关闭写端 - 重定向标准输入到读端
        execlp("wc", "wc", "-l", NULL);
    }
    else if(pid >0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO); //父进程向管道写数据 - 关闭读端 - 重定向标准输出到管道写端
        execlp("ls", "ls", NULL);
    }


    return 0;
}

