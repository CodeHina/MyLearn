class Solution {
public:
    int reverse(int x) 
    {
        //32位有符号整数 - -2^31 ~ 2^31-1
        //考虑整数溢出
        int ans = 0;
        while (x != 0) {
            int pop = x % 10;   //ans下一次要加的数

            //若下一次的结果数*10越界 或者 下一次的数乘10不越界但是(+-)pop越界 即溢出
            if (ans > INT_MAX / 10 || (ans == INT_MAX / 10 && pop > (INT_MAX) % 10)) 
                return 0;
            if (ans < INT_MIN / 10 || (ans == INT_MIN / 10 && pop < (INT_MIN) % 10)) 
                return 0;
                
            ans = ans * 10 + pop;
            x /= 10;
        }
        return ans;

    }

};