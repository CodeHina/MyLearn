//strtok函数的简单使用
//char* strtok(char* str, char* delim);
//函数用来将str字符串进行一次切分 - 每次切分以字符串delim为间隔
//返回值是切分出的字符串 - 直到切分完毕才返回NULL
//内部存在一个static变量 - 保存上次切分后剩余部分 - 其属于一个不可重入函数 
//信号捕捉函数不能设置为不可重入函数

#include <iostream>
#include <cstring>  

int main()
{
    char str[64] = "Hello,Friend,Welcome to my Github!";
    const char* delim = ",";

    char* token = strtok(str, delim);

    std::cout << token << std::endl; //返回被切分出来的字符串 - 分隔符之前的

    while(token != NULL)
    {
        token = strtok(NULL, delim); //由于继续切分原字符串 - 需传递NULL
        std::cout << token << std::endl;
    }

    return 0;
}

