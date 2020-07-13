class Solution
{
public:

/*
    思路和 2.两数相加 一致
    特别的地方是字符串是正序保存的 - 故每次迭代需要逆序输出 - 将字符的计算结果依次保存到res末尾 - 结果是逆序的
    故在rres逆序输出
*/
    string addStrings(string num1, string num2)
    {
        string res;

        short sum;
        short flag = 0;

        int i = num1.size() - 1, j = num2.size() - 1;
        while(i >= 0 || j >= 0)         //需要逆序遍历
        {
            char x = i >= 0 ? num1[i] : '0';
            char y = j >= 0 ? num2[j] : '0';

            //std::cout << "x = " << x << "y = " << y << std::endl;

            sum = short(x - '0') + short(y - '0') + flag;

            flag = sum / 10;
            sum = sum % 10;
            char c = char(sum + '0');

            res.push_back( c );

            if(i >= 0)
                --i;
            if(j >= 0)
                --j;
        }
        if(flag == 1)
            res.push_back(flag + '0');



        string rres = "";
        for(int i = res.size() - 1; i >= 0; --i)
        {
            rres += res[i];
        }

        return rres;
    }

};