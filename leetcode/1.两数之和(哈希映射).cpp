class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) 
    {
        //将数组组织为 num - index 的哈希表

        std::unordered_map<int, int> hash_map;

        for(int i = 0; i < nums.size(); ++i)
        {
            hash_map[nums[i]] = i;
        }//O(N)

        int i = 0;
        int y = -1;
        for(; i < nums.size(); ++i)
        {
            y = target - nums[i];
            if( hash_map.find(y) != hash_map.end() && hash_map.find(y)->second != i) //该元素存在 - 且不是当前元素
               break;
        }
         return {i, hash_map.find(y)->second};
    }
};