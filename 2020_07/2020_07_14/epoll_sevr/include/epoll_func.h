#ifndef __EPOLL_FUNC_H_
#define __EPOLL_FUNC_H_

struct epoll_data_func
{
    int fd;
    void* arg;
    void (*handle_events)(int fd,void* arg);
    
};



#endif
