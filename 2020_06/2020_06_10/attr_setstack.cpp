#include <iostream>
#include <pthread.h>
#include <unistd.h>



int main()
{

    pthread_attr_t attr;
    
    pthread_attr_init(&attr);//初始化一个线程属性

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置线程分离属性





    return 0;
}

