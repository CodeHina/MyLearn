//strtok_r() - 相比strtok()是可重入的,线程安全的
//char* strtok_r(char* str, char* delim, char** saveptr);
//新增传入传出参数saveptr用于保存当前的切分后的数据 - 不再使用静态变量


#include <iostream>
#include <cstring>

int main()
{
    char str[256] = "LiMing m 12,ZhangSan f 13,LiSi m 14"; 
    int StrNums = 0;    //总共分割的字符串数
    int PerNums = 0;    //外循环分割出的人数
    char* outp = NULL;  //外循环分割后的剩余字符串
    char* inp  = NULL;  //内循环分割后的剩余字符串
    char* info = NULL;
    char* pstr = str;   //利用指针变量pstr来代替str操作
    char* sbuf[32];

    while( (info = strtok_r(pstr, ",", &outp) ) != NULL)
    {
        //std::cout << "当前切分的字符为" << info << std::endl;
        //std::cout << "当前剩余的字符为" << outp << std::endl;

        char* pbuf = info;         //保存第StrNums个人的信息
        PerNums++;
        while( (sbuf[StrNums] = strtok_r(pbuf, " ", &inp)) )//切分每个人的信息
        {
            //std::cout << "i - 当前切分的字符为" << sbuf[StrNums] << std::endl;
            //std::cout << "i - 当前剩余的字符为" << inp << std::endl;
            StrNums++;
            pbuf = NULL;
        }
        pstr = NULL; //下次切分置为NULL
    }

    for(int i = 0; i < StrNums; i++)
    {
        if(i % PerNums == 0 && i != 0)
            std::cout << "\n";
        std::cout << sbuf[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

