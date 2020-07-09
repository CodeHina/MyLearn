#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../include/Server.h"
#include "../include/wrap_sock.h"
#include "../include/sys_err.h"
#include "../include/inet_const_val.h"

Server::Server() : MySocket(SOCKET_INIT), CommuSocket(SOCKET_INIT), num_of_link_client(0), port(SEVR_PORT)
{
    h_sock_addr = new sockaddr_in;                //初始化本机地址
    memset(h_sock_addr, 0, sizeof(sockaddr_in));
    
    l_sock_addr = new sockaddr_in;                 //缓存登录客户端的sockaddr
    memset(l_sock_addr, 0, sizeof(sockaddr_in) * MAX_LINK);
}


void Server::SetConfig()
{
    MySocket = Socket(AF_INET, SOCK_STREAM, 0);//采用ipv4协议以及面向字节流的TCP
/*
    int ret = inet_pton(AF_INET, SIP_ADDR,(void*)&n_ip_addr );//将字符串ip地址转换为网络上的ip地址
    if(ret == 0)
    {
        std::cerr << SIP_ADDR << " is not valid\n";//给定的点分十进制字符串ip地址无效
    }
    else
        SysErr("inet_pton",ret);
*/

    //配置服务器sockaddr
    h_sock_addr->sin_family = AF_INET;
    h_sock_addr->sin_port = htons(port);
    h_sock_addr->sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY为系统配置的本机可用的ip地址
    
    Bind(MySocket, (sockaddr*)h_sock_addr, sizeof(sockaddr_in));

    Listen(MySocket, 128);   //一块建立三次握手的客户端数量最大为128(128已经是可设置的最大值)

}

void Server::AcceptClient()
{
    socklen_t addrlen = sizeof(sockaddr_in);
    CommuSocket = Accept(MySocket, (sockaddr*)&l_sock_addr[num_of_link_client], &addrlen);
    
    char buf[128];

    std::cout   << "------" << inet_ntop(AF_INET, &l_sock_addr->sin_addr.s_addr, buf, sizeof(buf) )
                << ":" << ntohs(l_sock_addr->sin_port) << " log in------" << std::endl;   //需封装到函数中
}


Server::~Server()
{
    Close(MySocket);    //关闭套接字
    Close(CommuSocket);
    
    delete h_sock_addr;   //释放堆资源
    delete []l_sock_addr;
}


void Handle(int c_socket)
{
    //AcceptClient()返回后就已经和客户端建立连接 - 需依靠CommuSocket与该客户端通信
    char buf[BUFSIZ];
    memset(buf, 0 , sizeof(buf));

    if( Read(c_socket, buf, sizeof(buf)) == 0)               //读取客户端数据
    {
        //当前读取为0表示客户端已经关闭
        std::cout << "a client closed" << std::endl;
    }

    std::cout << "receive msg from Client : " <<  buf << std::endl;                      //需封装到一个显示函数中
 
    memset(buf, 0 , sizeof(buf));

    std::cout << "please input information to Client :\n";
    fgets(buf, sizeof(buf), stdin);

    Write(c_socket, buf, strlen(buf));    
}
