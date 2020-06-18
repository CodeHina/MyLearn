#include <iostream>
#include <unistd.h>

int main()
{

    //char* ttyname(int fd); - 返回某个文件描述符的终端
    std::cout << "fd(0) - " << ttyname(0) << std::endl;

    std::cout << "fd(1) - " << ttyname(1) << std::endl;

    std::cout << "fd(2) - " << ttyname(2) << std::endl;

    return 0;
}

