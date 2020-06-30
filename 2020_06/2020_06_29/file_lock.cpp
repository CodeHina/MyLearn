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

4.获取/设置当前文件的文件建议锁
    
    F_SETLK(trylock)\F_SETLKW(lock)\F_GETLK(可获取持有该锁的线程id) - (arg->struct flock*)
        
    struct flock{
        ...
        short l_type;       //锁类型 - F_RDLCK\F_WRLCK\F_UNLCK
        short l_whence;     //从文件哪个位置 - SEEK_SET\SEEK_CUR\SEEK_END
        off_t l_start;      //从当前位置可设置l_start偏移量
        off_t l_len;        //要锁住的字节数 - 特别滴(0时锁住整个文件)
        pid_t pid;          //持有此文件锁的进程id
        ...
    }
    
    强制锁
    open file description lock


5.管理信号
    
    F_GETOWN,F_SETOWN,F_GETOWN_EX,F_SETOWN_EX,F_GETSIG,F_SETSIG - 管理文件I/O信号

    F_GETOWN(arg->void) - 返回进程id或者进程组id(负的)当接受到SIGIO/SIGURG关于fd文件的事件

    F_SETOWN(arg->int) - ...待补充

6.返回值
    
    F_DUPFD - 新的文件描述符    F_GETFD - 文件描述符标志的值
    F_GETFL - 文件状态标志的值  F_GETOWN - 文件描述符拥有者的值
    F_GETSIG - 
    F_GETPIPESZ,F_SETPIPESZ - 管道的容量
    F_GET_SEALS - 
    其他命令为0 - -1,函数失败 - 并设置errno
 */

void sys_err(const char* str)
{
    perror(str);
    exit(-1);
}



int main(int argc, char* argv[])
{
    
    sleep(2);

    if(argc < 2)
    {
        sys_err("./a.out FileName");
    }
    
    int fd = open(argv[1], O_RDWR); /* 打开文件 */
    if(fd == -1)
        sys_err("open");

    size_t fsize = lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);

    struct flock f_lock; //定义文件锁 - 并设置相关属性
   
    //f_lock.l_type = F_RDLCK; //读锁
    f_lock.l_type = F_WRLCK; //写锁 - 两个进程间不可共享
    f_lock.l_whence = SEEK_SET;
    f_lock.l_start = 0;
    f_lock.l_len = fsize / 2; //从开头开始锁住文件的一半


    
    int flags = fcntl(fd, F_SETLKW, &f_lock );  //加锁 - 未获取则阻塞
    if(flags == -1)
        sys_err("fcntl");
    
    std::cout << "pid : " << getpid() << " hold a lock by fcntl()" << std::endl;
 
    sleep(10); //模拟对文件的操作

    f_lock.l_type = F_UNLCK;
    int flags2 = fcntl(fd, F_SETLKW, &f_lock );  //解锁
    if(flags2 == -1)
        sys_err("fcntl");
    std::cout << "fcntl unlock" << std::endl;   


    return 0;
}

