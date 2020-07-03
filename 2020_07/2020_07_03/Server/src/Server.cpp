#include "../include/Server.h"
#include "../include/inet_err.h"
#include "../include/inet_const_val.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



/* ---------------------------------------------------------------------------------- */
Server::Server() : MySocket(SOCKET_INIT), CommuSocket(SOCKET_INIT), num_of_link_client(0), port(SEVR_PORT)
{
    h_sock_addr = new sockaddr_in;                //初始化本机地址
    memset(h_sock_addr, 0, sizeof(sockaddr_in));
    
    l_sock_addr = new sockaddr_in;                 //缓存登录客户端的sockaddr
    memset(l_sock_addr, 0, sizeof(sockaddr_in) * MAX_LINK);
}


void Server::SetConfig()
{
    MySocket = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4协议以及面向字节流的TCP
    SysErr("socket",MySocket);

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
    
    SysErr("bind", bind(MySocket, (sockaddr*)h_sock_addr, sizeof(sockaddr_in)) );

    SysErr("listen" , listen(MySocket, MAX_LINK) ); //同时最大连接数为250

}

void Server::Accept()
{
    socklen_t addrlen = sizeof(sockaddr_in);
    CommuSocket = accept(MySocket, (sockaddr*)&l_sock_addr[num_of_link_client], &addrlen);
    SysErr("accpet", CommuSocket);
    
    char buf[128];

    std::cout   << inet_ntop(AF_INET, &l_sock_addr->sin_addr.s_addr, buf, sizeof(buf) )
                << ":" << ntohs(l_sock_addr->sin_port) << " log in" << std::endl; 
}

void Server::handle()
{
    //Accept()返回后就已经和客户端建立连接 - 需依靠CommuSocket与该客户端通信
    char buf[BUFSIZ];
    memset(buf, 0 , sizeof(buf));

    int rdbytes = read(CommuSocket, buf, sizeof(buf)); //读取客户端数据
    
    std::cout << "read_bytes : " << rdbytes << std::endl;
    std::cout << buf << std::endl; //需封装到一个显示函数中

    

    //SysErr("write", write(CommuSocket, buf, strlen(buf)) );    


}

Server::~Server()
{
    close(MySocket);    //关闭套接字
    close(CommuSocket);
    
    delete h_sock_addr;   //释放堆资源
    delete []l_sock_addr;
}


