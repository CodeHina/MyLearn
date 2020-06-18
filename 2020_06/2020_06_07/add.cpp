#include <iostream>

#define C_ADD


int add(int n1, int n2)
{

#ifdef C_ADD
    /*
       普通加法是考虑进位值 - 进位值 * 10 + 不进位值

       位操作
            1.a ^ b 异或操作 - 如 5 ^ 4 - 0101 ^ 0100 - 得到无需进位的值 - 0001(c)
            2.a & b 与操作   - 如 5 & 4 - 0101 & 0100 - 得到同样为1的位 - 也就是需要进位的位 0100 - 进1位 0100 << 1 - 01000(d)
            3.c与d再继续重复1/2步骤 
                00001 ^ 01000 - 010001
                00001 & 01000 - 000000 << 1 - 00000

                显然010001(9)即为所求 - 故退出条件是无需进位时(进位后值为0)
    */

    while(0 != n2)
    {
        int temp = n1;        //备份n1用于&运算
        n1 = n1 ^ n2;         //保存无需进位值 - 即最后的结果
        n2 = (temp & n2) << 1;//保存需要进位的值
    }
    
    return n1;

#endif

#ifdef  ASM     //还可通过汇编##滑稽
    _asm
    {
        MOV EAX, n1
        MOV ECX, n2
        ADD EAX, ECX
    }
#endif


}

int main()
{
    int n1, n2;
    std::cout << "n1 = ";
    std::cin >> n1;

    std::cout << "n2 = ";
    std::cin >> n2;
    
    std::cout << "不通过四则运算的加法 - n1 + n2 = " << add(n1, n2) << std::endl;
}


