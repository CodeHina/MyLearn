#include <iostream>
#include <cstring>
#include <cctype>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/wrap_sock.h"

#define MAX_EVENTS 1024 //监听上限
#define SEVR_PORT  1234

void recv_data(int, int, void*);
void send_data(int, int, void*);

struct myevents
{
    int     fd;                                                     //要监听的文件描述符
    int     event;                                                  //所对应的监听事件
    void    *arg;                                                   //回调函数参数
    void    (*call_back)(int fd, int evt, void* arg);               //事件触发时的回调函数
    int     moni_status;                                            //监听状态 - 是否在红黑树上 - 0/1
    char    buf[BUFSIZ];                                            
    size_t  len;                                                
    long    last_active;                                            //记录每次在红黑树上操作的时间
};

int g_efd;                                                          //全局变量 - 保存epoll_create返回的文件描述符
myevents g_myevts[MAX_EVENTS + 1];                                  //监听事件数组

/* 将当前文件描述符对应的epoll_event::data.ptr进行设定 */
void event_set(myevents* ev, int fd, void(*call_back)(int, int, void*), void* arg)
{
    ev->fd = fd;
    ev->event = 0;
    ev->arg = arg;
    ev->call_back = call_back;
    ev->moni_status = 0;
    //memset(ev->buf, 0, sizeof(ev->buf));
    //ev->len = 0;
    ev->last_active = time(nullptr);        //更新当前时间
}

/*  将当前的文件描述符添加到红黑树上  */
void event_add(int efd, int event, myevents* ev/*I/O*/)
{
    epoll_event epv = {0, {0}}; //此文件描述符对应的epoll_event
    int op;//对此文件描述符的操作方式

    epv.data.ptr = ev;  //构造此事件
    epv.events = ev->event = event;
    

    if(ev->moni_status == true)
        op = EPOLL_CTL_MOD;   //此文件描述符已经在监听树上 - 修改即可 
    else
        op = EPOLL_CTL_ADD;   //不在则添加

    if(epoll_ctl(efd, op, ev->fd, &epv) < 0)
    {
        std::cout << "-----\nevent ctl(add/mod) failed at fd : " << ev->fd;
        switch(epv.events)
        {
        case EPOLLIN:
            std::cout << " at EPOLLIN\n-----" << std::endl; break;
        case EPOLLOUT:
            std::cout << " at EPOLLOUT\n-----" << std::endl; break;
        }
        perror("epoll_ctl_add err");
    }
    else
    {
        ev->moni_status = 1;
        std::cout << "-----\nevent ctl(add/mod) success at fd : " << ev->fd;
        switch(epv.events)
        {
        case EPOLLIN:
            std::cout << " at EPOLLIN\n-----" << std::endl; break;
        case EPOLLOUT:
            std::cout << " at EPOLLOUT\n-----" << std::endl; break;
        }
    }
    return;
}

void event_del(int efd, myevents* ev)
{
    
    epoll_event epv = {0, {0}};

    if(ev->moni_status == 0)//已经不在树上
        return;
    
    epv.data.ptr = ev;
    epv.events = ev->event;
    ev->moni_status = 0;    //将要操作的epoll_event是将的参数进行填充 - 更改此事件的监听状态(从监听树上取下)

    int op = EPOLL_CTL_DEL;
    
    if(epoll_ctl(efd, op, ev->fd, &epv) < 0)
    {
        std::cout << "-----\nevent ctl(del/mod) failed at fd : " << ev->fd;
        switch(epv.events)
        {
        case EPOLLIN:
            std::cout << " at EPOLLIN\n-----" << std::endl; break;
        case EPOLLOUT:
            std::cout << " at EPOLLOUT\n-----" << std::endl; break;
        }
        perror("epoll_ctl_del");
    }
    else
    {
        std::cout << "-----\nevent ctl(del/mod) success at fd : " << ev->fd;
        switch(epv.events)
        {
        case EPOLLIN:
            std::cout << " at EPOLLIN\n-----" << std::endl; break;
        case EPOLLOUT:
            std::cout << " at EPOLLOUT\n-----" << std::endl; break;
        }
    }
}

//evt->call_back(evt->fd, ready_evts[i].events, evt->arg);
void accept_conn(int fd, int evt, void* arg) //lfd的回调函数 - 每当有客户端连接触发此事件
{
    //fd == lfd - evt == EPOLLIN - arg(lfd对应的myevents)
    sockaddr_in link_client_addr;
    socklen_t addr_len = sizeof(link_client_addr);

    int cfd = -1;//与指定客户端进行通信socket

    if( (cfd = Accept(fd, (sockaddr*)&link_client_addr, &addr_len)) < 0 )
    {
        if(errno != EAGAIN || errno != EINTR) //非阻塞的lfd - 未收到连接请求则返回-1并设置errno 或者 被信号中断
        {

        }
        //Accept()已经封装其他出错处理 - in wrap_sock.cpp
    }
    //成功建立一个连接
    int avai_client_pos;//g_myevts中的一个可用位置
    do
    {
        for(avai_client_pos = 0; avai_client_pos < MAX_EVENTS; ++avai_client_pos)
        {
            if(g_myevts[avai_client_pos].moni_status == 0)//当前位置的事件还未被监听
                break;                                    //故可以被使用作为当前连接客户端的一个监听事件
        }   
        if(avai_client_pos == MAX_EVENTS) //无可用位置
        {
            std::cerr << "too many connect,max connect client is " << MAX_EVENTS << std::endl;
            break; //跳出循环 - 不执行下边代码 - do while(0)相当于一个goto语句
        }

        int flag = 0; //将监听已连接客户端请求的cfd的设置为非阻塞
        if( (flag = fcntl(cfd, F_SETFL, SOCK_NONBLOCK)) < 0)
        {
            perror("fcntl err - set SOCK_NONBLOCK failed");
        }

        //设置此cfd的事件 - myevents - 其回调函数为recv_data - 监听cfd的可读事件 - 即客户端向服务端的写事件
        event_set(&g_myevts[avai_client_pos], cfd, recv_data, &g_myevts[avai_client_pos]);
        event_add(g_efd, EPOLLIN, &g_myevts[avai_client_pos]);


    }while(0);

    //打印此客户端的相关信息 - 以及自动向客户端回写欢迎语句
    char client_addr[INET_ADDRSTRLEN];
    const char* addr = inet_ntop(AF_INET, &link_client_addr.sin_addr, client_addr, sizeof(client_addr));
    std::cout << "client:" << addr << " - port:" << ntohs(link_client_addr.sin_port) << " log in" << std::endl;

    send(cfd, "****welcome to log in server****", sizeof("****welcome to log in server****"), MSG_DONTWAIT);
}

void init_lisent_socket(int efd, int port)
{
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, SOCK_NONBLOCK);         //设置为非阻塞

    /* void event_set(myevents*, int, void(*call_back)(int, int, void*), void*) */
    //设置lfd对应事件的结构体 - 初始化其各项指标 - 对g_myevts[MAX_EVENTS]对应的myevents进行初始化
    event_set(&g_myevts[MAX_EVENTS], lfd, accept_conn, &g_myevts[MAX_EVENTS]);

    /* void event_add(int, int, myevents*) */
    //将当前的文件描述符添加到红黑树上 - 
    event_add(efd, EPOLLIN, &g_myevts[MAX_EVENTS]);

    sockaddr_in sevr_addr; //服务器地址地址参数
    memset(&sevr_addr, 0, sizeof(sevr_addr));
    sevr_addr.sin_family = AF_INET;
    sevr_addr.sin_port = htons(port);
    sevr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(lfd, (sockaddr*)&(sevr_addr), sizeof(sevr_addr));//将socket和服务器端口ip地址等信息进行绑定

    Listen(lfd, 128); //同时建立连接的客户端个数
}


int main(int argc, char* argv[])
{
    unsigned short port = SEVR_PORT;

    if(argc == 2)
        port = atoi(argv[1]);           //用户自定义端口

    g_efd = epoll_create(MAX_EVENTS + 1);
    if(g_efd == -1)
        { perror("epoll_create err"); exit(-1); }

    memset(g_myevts, 0, sizeof(g_myevts));//全部数据置为0

    init_lisent_socket(g_efd, port);    //初始化监听socket

    epoll_event ready_evts[MAX_EVENTS + 1];     //保存就绪的文件描述符


    std::cout << "sever wait client to link........" << std::endl;

    while(1)
    {
        /* 超时验证 - 每次检测100个连接的客户端 - 当客户端60s内和服务器无数据通信 - 即关闭客户端连接 */




        /* 监听红黑树g_efd - 将满足事件的文件描述符加入到ready_evts,只阻塞1s - 无事件满足即返回 - 轮询即可 */
        int ready_fds = epoll_wait(g_efd, ready_evts, MAX_EVENTS + 1, 1000);
        if(ready_fds == -1)
            { perror("epoll_wait err"); exit(-1); }

        

        for(int i = 0; i < ready_fds; ++i)
        {

            /* epoll_event - event/data */
            /* 接收就绪事件中的data.ptr - 泛型指针 */
            myevents* evt = (myevents*)ready_evts[i].data.ptr;

            if(ready_evts[i].events & EPOLLIN && (evt->event) & EPOLLIN) //都满足且为读就绪事件
            {
                evt->call_back(evt->fd, ready_evts[i].events, evt->arg);
            }
            if(ready_evts[i].events & EPOLLOUT && (evt->event) & EPOLLOUT)
            {
                evt->call_back(evt->fd, ready_evts[i].events, evt->arg);
            }
        }
    }



    /* 注意释放所有资源 */


    return 0;
}

void recv_data(int fd, int evt, void* arg)
{
    myevents* recv_evt = (myevents*)arg; //将满足事件的描述符所对应的事件结构体取出

    /* 读取文件描述符中数据 - 将其写入此文件描述符事件的myevents的缓冲区中 */
    int rdbytes = recv(fd, recv_evt->buf, sizeof(recv_evt->buf), 0); 
    
    /* 由于本服务器的特性是读取客户端数据后便向客户端发送一个消息 */
    /* 由于滑动窗口的限制 - 若客户端的接收窗口内的数据是满的,即服务器向客户端写数据可能会阻塞 -  */
    event_del(g_efd, recv_evt);              //将此文件描述符节点摘除
    
    if(rdbytes > 0)
    {
        recv_evt->len = rdbytes; //本次读到的字节数
        recv_evt->buf[recv_evt->len] = '\0';
        for(int i = 0; i < rdbytes; ++i)//依次转为大写
            toupper(recv_evt->buf[i]);
        
        //std::cout << " the msg in recv_evt->buf is " << recv_evt->buf 
        //          << " recv from fd : " << fd << std::endl;

        event_set(recv_evt, recv_evt->fd, send_data, recv_evt); //将当前文件描述符的监听事件改为写并加到监听树上
        event_add(g_efd, EPOLLOUT, recv_evt);

    }
    else if(rdbytes == 0) //客户端关闭 - 读到的数据为0
    {
        shutdown(recv_evt->fd, SHUT_RDWR);
        std::cout << "\n##pos: " << recv_evt - g_myevts << " fd: " << recv_evt->fd << " closed" << std::endl;
    }
    else    //其他错误 - EINTR/EWOULDBLOCK
    {
        shutdown(recv_evt->fd, SHUT_RDWR);
        std::cout << "\n##pos: " << recv_evt - g_myevts << " fd: " << recv_evt->fd << " err" << std::endl;
        perror("recv err");
    }

}

void send_data(int fd, int evt, void* arg)
{
    myevents* send_evt = (myevents*)arg;
    
    //std::cout << "send_evt_addr is " << send_evt << std::endl;

    //std::cout << "---wait send buf is " << send_evt->buf << " len is " << send_evt->len << std::endl;

    int wrbytes = send(fd, &send_evt->buf, send_evt->len, 0);
    
    event_del(g_efd, send_evt);

    if(wrbytes >= 0)
    {
        //std::cout << "send to " << "fd:" << send_evt->fd << " " << wrbytes << "bytes" << std::endl;
        /* 满足可写后调用此函数 - 然后重写监听可读事件 */
        event_set(send_evt, fd, recv_data, send_evt);
        event_add(g_efd, EPOLLIN, send_evt);
    }
    else //读取错误 - EINTR/EAGAIN等错误需要特殊处理
    {
        shutdown(send_evt->fd, SHUT_RDWR);
        std::cout << " send to " << "fd: " << send_evt->fd << " err"  << " wrbytes: " << wrbytes << std::endl;
        perror("send err");
    }
}
