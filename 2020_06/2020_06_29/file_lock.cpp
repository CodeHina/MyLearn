#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//利用int fcntl(int fd, int cmd, ... /*arg*/);实现对文件加锁 - 读锁/写锁/解锁
//进程间的文件锁仍然满足 - 读时共享,写时独占,写锁优先级高

/*
    fcntl() - 参数cmd
    
1.拷贝一份文件描述符
    
    F_DUPFD(arg->int) - 拷贝当前fd指向文件描述符(返回一个大于等于arg的(且可用的最小的)文件描述符) - 相较于dup2()函数更加的精确指定

    F_DUPFD_CLOEXEC(arg->int) - 此位若设置为1,则成功调用exec()类函数后会关闭此文件描述符

2.获取/设置当前的文件的文件描述符标志
    
    F_GETFD(arg->void) - 返回当前文件的文件描述符标志(File descriptor flags)

    F_SETFD(arg->int) - 设置当前文件的文件描述符标志为arg

    多线程中 - 利用fcntl()/F_SETFD来设置close_on_execv

3.获取/设置当前文件的文件状态标志

    F_GETFL(arg->void) - 返回当前文件的文件状态标志(如由open()函数设置的O_RDWR)
    
    F_SETFL(arg->int) - 设置当前文件的文件状态标志(通过与运算 - 设置某个标志位)
        如File acess mode(O_RDONLY O_WRONLY O_RDWR) - File creation flags(O_CREAT O_EXCL O_TRUNC O_NOCTTY)
        O_ASYNC O_DIRECT O_NOATIME O_NONBLOCK(非阻塞)

4.


 */

void sys_err(char* str)
{
    perror(str);
    exit(-1);
}



int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        sys_err("./a.out FileName");
    }
    
    int fd = open(argv[1], O_RDWR); /* 打开文件 */
    
    int flags = fcntl();

    return 0;
}

