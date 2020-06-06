#include<iostream>


using namespace std;


int main(void)
{
    char str1[] = "hello world";   //存放在栈中的数组
    char str2[] = "hello world";   //存放在栈中的数组

    const char str3[] = "hello world";  //存放在栈中的字符串常量
    const char str4[] = "hello world";  //存放在栈中的字符串常量

    const char* pstring1 = "hello world";   //本身在栈中，指向常量的指针
    const char* pstring2 = "hello world";   //本身在栈中，指向常量的指针     //显然二者所指向的地址一致

    int x = (int)pstring1;
    int y = (int)pstring2;                  //为了方便打印出指针所指向的地址

    cout << boolalpha << ( str1==str2 ) << endl;               //比较字串首地址      flase
    cout << boolalpha << ( str3==str4 ) << endl;               //比较字串首地址      flase
    cout << boolalpha << ( pstring1==pstring2 ) <<endl;        //比较指针所指地址    true

    cout << "str1=" << &str1 << ",";
    cout << "str2=" << &str2 << endl;

    cout << "str3=" << &str3 << ",";
    cout << "str4=" << &str4 << endl;

    cout << "pstring1=" << &pstring1 << ",";
    cout << "pstring2=" << &pstring2 << endl;                   //输出指针本身地址

    cout<<hex;
    cout << "pstring1=" << x << ",";
    cout<<hex;
    cout << "pstring2=" << y << endl;      //16进制输出指针所指地址

    return 0;
}
