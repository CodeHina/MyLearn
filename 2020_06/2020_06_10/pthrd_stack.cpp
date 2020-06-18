#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

void* tfunc(void* arg)
{
    void* stackaddr;
    size_t statcksize = 10;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_getstack(&attr, &stackaddr ,&statcksize);

    std::cout << "size - " << statcksize << std::hex << " --- 0x" << stackaddr << std::endl;

    

    while(1)
        sleep(1);
}


int main()
{
    pthread_t tid;
    int count = 0;

    while(1)
    {
        int ret =  pthread_create(&tid, NULL, tfunc, NULL);
        if( 0 != ret )
        {
            std::cerr << "pthread_create : " <<  strerror(ret) << std::endl;
            break;
        }

        ++count;
    }

    std::cout << "最多创建" << count << "个线程" << std::endl;

    return 0;
}

