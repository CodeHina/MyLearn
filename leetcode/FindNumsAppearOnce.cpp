class Solution {
public:
    void FindNumsAppearOnce(vector<int> data,int* num1,int *num2) 
    {
        //出现两次 - 出现一次
        //异或的性质 - 一个数异或本身结果为0
        //故可以知道 - 此数组所有数字异或一遍不为0 - 且为两个只出现一次的数的异或结果
        //异或 - 相同为0,不同为1 - 两个只出现一次的数字的二进制位相互对比至少有一个位不同（一个为0/另外一个为1）
        //可以根据最低位的不同将数组的数分为两个种类 - 第N位为1或为0的 - 分别异或 - 即可得到两个数字
        
        int num = data[0];
        for(int i = 1; i < data.size(); ++i)
        {
            num ^= data[i];
        }
        
        //找到第n位 - 即第一个为1的位
        int n = 1;//001
        while( (num & 1) == 0 ) //从第1位开始考察 - &操作为0，则右移动一位
        {
            n++;
            num >>= 1;
        }
        int flag = 1 << (n -1);//只有第n位为1
        
        for(int i = 0; i < data.size(); ++i)
        {
            if( (data[i] & flag) != 0 )//第n位为1的数 & flag 必然不为0
                *num1 ^= data[i];
            else
                *num2 ^= data[i];
        }
        
        
        
    }
};