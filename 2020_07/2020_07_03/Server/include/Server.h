#ifndef __SERVER_H_
#define __SERVER_H_

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

class Server
{
public:
    Server();
    ~Server();

    void SetConfig();       //设置服务端的基本配置
    void AcceptClient();    //等待客户端连接
    void Handle();          //处理数据

private:
    
    int      MySocket;          //需配置的套接字
    int      CommuSocket;       //与客户端通信的套接字
    int      num_of_link_client;//连接的客户端数量
    uint16_t port;              //此服务所需端口
    uint32_t n_ip_addr;         //网络字节序服务器ip地址
    struct sockaddr_in* h_sock_addr;//本机的sockaddr
    struct sockaddr_in* l_sock_addr;//登录本机的客户端的sock_addr;
};





#endif

