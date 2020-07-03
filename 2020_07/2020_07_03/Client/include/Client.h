#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <cstdint>



class Client
{
public:
    Client();
    ~Client();

    void SetConfig(); //设置客户端的基本配置
    void Connect();    //连接服务端
    void handle();    //处理数据

private:
    
    int      CommuSocket;       //与客户端通信的套接字

    uint16_t port;              //此服务所需端口
    uint32_t n_ip_addr;         //网络字节序的服务器ip地址
   
    struct sockaddr_in* n_sock_addr;//要连接的服务器的sockaddr
};





#endif

