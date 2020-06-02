#include <iostream>
#include <unistd.h>

using namespace std;


int main()
{
    pid_t pid = fork();
    
    if(pid == 0)
    {
        cout << "Child pid : " << getpid() << " ppid : " << getppid() << " pgrp : "  << getpgrp();
        sleep(5);   //等待父进程修改自己的组id

        
        cout << "\nafter change :" << endl;
        cout << "Child pid : " << getpid() << " ppid : " << getppid() << " pgrp : "  << getpgrp();
            
    }
    else if( pid > 0 )
    {
        sleep(1);

        //设置子进程的进程组id - 为子进程的id
        if( setpgid(pid, pid) == -1)
            perror("c - setpgid");
        
        sleep(10);
        cout << "\n---------\nParent pid : " << getpid() << " ppid : " << getppid() << " pgrp : "  << getpgrp();

        //设置父进程进程组id为父进程的父进程(bash)的组id
        if( setpgid(getpid(), getppid()) == -1)
            perror("p - setpgid");
        
        cout << "\nafter change :" << endl;
        cout << "Parent pid : " << getpid() << " ppid : " << getppid() << " pgrp : "  << getpgrp();
        
        while(1);
        cout << endl;
    }



    return 0;
}

