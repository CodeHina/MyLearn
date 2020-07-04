#include "../include/wrap_sock.h"
#include "../include/sys_err.h"
#include "../include/inet_const_val.h"
#include "../include/Client.h"

#include <iostream>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client() : CommuSocket(SOCKET_INIT), port(SEVR_PORT)
{
    n_sock_addr = new sockaddr_in;                //初始化本机地址
    memset(n_sock_addr, 0, sizeof(sockaddr_in));
}


void Client::SetConfig()
{
    CommuSocket =  Socket(AF_INET, SOCK_STREAM, 0); //已封装出错处理
}

void Client::ConnectServer()
{
    int ret = inet_pton(AF_INET, SEVR_IP_ADDR, (void*)&n_ip_addr); 
    if(ret == 0)
    {
        std::cout << "服务器ip地址失效" << std::endl;
    }
    else if(ret == -1)
        perr_exit("inet_pton");

    n_sock_addr->sin_family = AF_INET;
    n_sock_addr->sin_port = htons(port);
    n_sock_addr->sin_addr.s_addr = n_ip_addr;
 
    Connect(CommuSocket, (sockaddr*)n_sock_addr, sizeof(*n_sock_addr));
}

void Client::Handle()
{
    //ConnectServer()返回后就已经和服务器建立连接 - 需依靠CommuSocket与该服务器通信
    char buf[BUFSIZ];
    memset(buf, 0 , sizeof(buf));
/*
    int rdbytes = read(CommuSocket, buf, sizeof(buf)); //读取服务器数据
    
    std::cout << "read_bytes : " << rdbytes << std::endl;
    std::cout << buf << std::endl; //需封装到一个显示函数中

    memset(buf, 0, sizeof(buf));
    strcpy(buf, "hello,server!");
*/
    std::cout << "please input some information to Server :\n";
    fgets(buf, sizeof(buf), stdin); //读取一行stdin的用户输入
    Writen(CommuSocket, buf, strlen(buf));

    memset(buf, 0 , sizeof(buf));
    Read(CommuSocket, buf, sizeof(buf));

    std::cout << "receive msg from Server : " << buf << std::endl;
}

Client::~Client()
{
    Close(CommuSocket);
    
    delete n_sock_addr;   //释放堆资源
}


