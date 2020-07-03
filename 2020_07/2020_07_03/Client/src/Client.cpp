#include "../include/Client.h"
#include "../include/inet_err.h"
#include "../include/inet_const_val.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



/* ---------------------------------------------------------------------------------- */
Client::Client() : CommuSocket(SOCKET_INIT), port(SEVR_PORT)
{
    n_sock_addr = new sockaddr_in;                //初始化本机地址
    memset(n_sock_addr, 0, sizeof(sockaddr_in));
}


void Client::SetConfig()
{
    CommuSocket = socket(AF_INET, SOCK_STREAM, 0); //TCP/IP
    SysErr("socket", CommuSocket);
}

void Client::Connect()
{
    int ret = inet_pton(AF_INET, SEVR_IP_ADDR, (void*)&n_ip_addr); 
    if(ret == 0)
    {
        std::cout << "服务器ip地址失效" << std::endl;
    }
    else
        SysErr("inet_pton", ret);

    n_sock_addr->sin_family = AF_INET;
    n_sock_addr->sin_port = htons(port);
    n_sock_addr->sin_addr.s_addr = n_ip_addr;
    
    SysErr("connect", connect(CommuSocket, (sockaddr*)(n_sock_addr), sizeof(sockaddr_in)) );
}

void Client::handle()
{
    //Connect()返回后就已经和服务器建立连接 - 需依靠CommuSocket与该服务器通信
    char buf[1024];
    memset(buf, 0 , sizeof(buf));
/*
    int rdbytes = read(CommuSocket, buf, sizeof(buf)); //读取服务器数据
    
    std::cout << "read_bytes : " << rdbytes << std::endl;
    std::cout << buf << std::endl; //需封装到一个显示函数中

    memset(buf, 0, sizeof(buf));
    strcpy(buf, "hello,server!");


*/
    std::cout << "please input some information :\n";
    char* msg = fgets(buf, sizeof(buf), stdin); //从标准输入读取数据
    if(msg == NULL)
        SysErr("fgets", -1);

    SysErr("write", write(CommuSocket, msg, strlen(msg)) );

    memset(buf, 0, sizeof(buf)); //清空缓冲区

    SysErr("read", read(CommuSocket, buf, sizeof(buf)) );

    std::cout << "read msg from " << SEVR_IP_ADDR << " is " << buf << std::endl;


}

Client::~Client()
{
    close(CommuSocket);
    
    delete n_sock_addr;   //释放堆资源
}


