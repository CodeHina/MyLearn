#ifndef __CLIENT_H_
#define __CLIENT_H_


typedef unsigned short uint16_t;
typedef unsigned int uint32_t;



class Client
{
public:
    Client();
    ~Client();

    void SetConfig();           //设置客户端的基本配置
    void ConnectServer();       //连接服务端
    void Handle();              //处理数据

private:
    
    int      CommuSocket;       //与客户端通信的套接字

    uint16_t port;              //此服务所需端口
    uint32_t n_ip_addr;         //网络字节序的服务器ip地址
   
    struct sockaddr_in* n_sock_addr;//要连接的服务器的sockaddr
};




#endif

