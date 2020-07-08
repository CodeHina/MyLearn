#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::string;


class Solution {
public:

    Solution() {}
    ~Solution() {}

    string longestCommonPrefix(vector<string>& strs) 
    {
        //当前数组为空即返回空字符串
        //将首个字符串作为参考对象,和后面的字符串的每个字符依次对比
        if(strs.empty())
            return "";

        if(strs.size() <= 1)//仅有一个字符串,自然为所有的最长公共子前缀
        {
            return strs[0];
        }

        int min_len = strs[0].size();

        std::cout << "min_len : " << min_len << std::endl;

        for(int i = 1; i < strs.size(); ++i)
        {
            if(min_len > strs[i].size())
                min_len = strs[i].size();
        }
        
        std::cout << "min_len : " << min_len << std::endl;

        std::string str0 = strs[0]; //参考对象
        std::string res = "";       //待返回的对象

        int min_public_len = get_min_public_len(str0, strs[1], min_len);
        int i = 2;
        while(i < strs.size())
        {
            int temp_len = get_min_public_len(str0, strs[i++], min_len);
            if(min_public_len  > temp_len)
                min_public_len = temp_len;
        }

        std::cout << "min_public_len : " << min_public_len << std::endl;

        for(int i = 0; i < min_public_len; ++i)
            res += str0[i];

        return res;
    }

    int get_min_public_len(const string& s1, const string& s2, int len)
    {
        int i = -1;
        for(i = 0; i < len; ++i)
        {
            if(s1[i] != s2[i])
                return i;
        }
        return i;   //返回相等的字符个数
    }

};


int main()
{
    vector<string> strs = {"abca", "abc"};


    Solution s;
    std::cout << s.longestCommonPrefix(strs) << std::endl;

}













