//多线程拷贝一份文件

#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
    int i;      //线程编号
    int sfd;    //线程要打开的源文件的文件句柄
    int dfd;    //线程要打开的目标文件的文件句柄
    size_t copy_size;//该线程要拷贝的字节数
}copy_file_t;

void part_str_err(char*);//strtol()函数的解析字符串提示函数
void* tfunc(void*);      //线程主控逻辑函数


int main(int argc, char* argv[])
{
    
    if(argc < 3)
    {
        std::cout << "./a.out src.txt dest.txt Nthread" << std::endl;
        return -1;
    }

    //long int strtol(const char *nptr, char **endptr, int base);
    //将nptr指向的字符数转换为base进制的long型数 - endptr指向结束转换的位置
    char* end;
    long temp = strtol(argv[3], &end, 10);     //要创建的线程个数
    const int N = (int)temp;
    part_str_err(end);


    //打开源文件 - 获取源文件大小
    int sfd = open(argv[1], O_RDONLY);
    size_t fsize = lseek(sfd, 0, SEEK_END) - lseek(sfd, 0, SEEK_SET);//获取文件大小
    //打开目标文件 - 并截断为源文件大小
    int dfd = open(argv[2], O_RDWR | O_CREAT, 0644);
    truncate(argv[2], fsize);


    //分配源文件的要拷贝的字节到每个线程
    std::cout << "源文件大小为 - " << fsize << "bytes\n";

    size_t delta = fsize / N;  //每个线程所需拷贝的字节数
    size_t endsize = fsize - (N - 1) * delta;//最后一个线程需拷贝的字节数

    
    
    //创建线程
    pthread_t tid[N];       //线程id管理数组
    copy_file_t *cp_msg = new copy_file_t[N];

    for(int i = 0; i < N; ++i)
    {
        cp_msg[i].i = i;
        cp_msg[i].sfd = sfd;      //获取当前文件描述符
        cp_msg[i].dfd = dfd;
        if(i < N-1)
        {
            cp_msg[i].copy_size = delta;
        }
        else
        {
            cp_msg[i].copy_size = endsize;
        }//分配每个线程需要的信息
        pthread_create(&tid[i], NULL, tfunc, (void*)&cp_msg[i]);
    }





    for(int i = 0; i < N; ++i)  //回收结束的线程
    {
        if(0 == pthread_join(tid[i], NULL));
    }

    delete []cp_msg;            //释放堆内存资源
    pthread_exit(NULL);         //主控线程退出
}


void part_str_err(char* end)
{

    if('\0' == *end)
    {
        std::cout << "字符串解析完毕" << std::endl;
    }
    else 
    {
        std::cout << "argv[3] - 解析 - end at "  << *end << std::endl;
    }
}

void* tfunc(void* arg)
{
    copy_file_t* pcp_msg = (copy_file_t*)arg;
    
    //std::cout << pcp_msg->i << "线程要拷贝的字节数为 - "  << pcp_msg->copy_size << std::endl;


    lseek(pcp_msg->sfd, pcp_msg->i * pcp_msg->copy_size, SEEK_SET);    
    lseek(pcp_msg->dfd, pcp_msg->i * pcp_msg->copy_size, SEEK_SET);    

    char buf[512];
    int rbytes = read(pcp_msg->sfd, buf, sizeof(buf));
    while(rbytes != 0)
    {
        write(pcp_msg->dfd, buf, rbytes);
        rbytes = read(pcp_msg->sfd, buf, sizeof(buf));

        usleep(10);
    }


    std::cout << "-----------%";

    pthread_exit(NULL);

}
