class Solution {
public:
    int lengthOfLongestSubstring(string s) 
    {
        //数组 - 查找 - 判断操作
        //无序数组的唯一化 - O(n^2)
        //先sort再利用利用有序数组的唯一化(O(nlogn))
        //特别的 - 此题要求为字串 - 也就是连续的 - 那么若当前字符在前缀子串中有/则需从与其重复字符的位置的后一个字符重新计算

        if(s.size() == 0)   //空串
            return 0;

        string res;         //res不断吞噬s - 当前能够查找的前缀
        char c;             //当前字符

        res += s[0];        //初始化为第一个字符
        std::string::size_type index = 1;               //s的索引位置
        std::string::size_type str_begin = 0;           //子串的首字符索引位置
        std::string::size_type str_end = 0;             //子串的尾字符索引位置 - [begin,end]

        std::string::size_type max_len = str_end - str_begin + 1; //初始化最大子串长
        std::string::size_type len = 0;                           //当前计算的子串长
        //std::string::size_type pos = 0;                         //查找的起始位置

        //关键在于 - 查找子串的确定 - str_end始终指向当前字符 - 只要将str_begin移动到重复字符的后一个位置即可确定了查找区间
        //查找的起始位置pos也要在str_begin更新后及时更新为str_begin

        while(index != s.size())
        {
            char cur_c = s[index];
            std::string::size_type temp_pos = res.find(cur_c, str_begin);//当前字符在res中的查找结果 - 其重复字符的索引位置

            if( temp_pos != res.npos )//当前字符在结果字符串存在
            {
                str_begin = temp_pos + 1;    //则当前字符的重复字符要舍去 - 故从重复字符位置后一个字符位置重新计数
                str_end = str_begin; 

                //pos = temp_pos + 1;          //当前的查找区间也要更新 - 起始位置始终为有效字串的首位置
            
                res.append(1,cur_c);         //吞噬当前字符 - res始终为s的[0,index]字串 - 查找区间为[pos,index]
                str_end = index;             //str_end也要更新为当前字符位置 - 查找区间为[pos,str_end/index]
            }
            else
            {
                res.append(1,cur_c);    //追加到结果字符串末尾
                str_end = index;        //str_end也要更新为当前字符位置 - 查找区间为[pos,str_end/index]

                len = str_end - str_begin + 1;    //当前的子串长度
                max_len = more_long(len, max_len);//更新最长子串长度
            } 

            ++index;
        }

        //len = str_end - str_begin + 1;//当前的子串长度
        //max_len = more_long(len, max_len);//更新最长子串长度

        return max_len;
    }

    int more_long(int len, int max_len)
    {
       return  std::max(len,max_len);
    }

    /*
        //错误 - 求的是不重复字符数
            string res; //结果字符串
            char c;     //当前字符

            res += s[0];
            std::string::size_type index = 1;
            while(index != s.size())
            {
                char cur_c = s[index++];
                if( res.find(cur_c) != res.npos )//当前字符在结果字符串存在
                    continue;//略过此字符
                res.append(1,cur_c);//追加到结果字符串末尾
            }

            return res.size();
    */
};