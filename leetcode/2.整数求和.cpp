/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
/*
    考虑链表过长必然溢出 - 故采用小学计算加法模型 - 进位加法
    由于两个链表长度可能不等 - 由计算方法可知短的必然需要补零
    故方法1：
        计算两个链表长度 - 需要O(m+n)
        对短链表添加节点补0 - O(m-n)
        然后按照长链表长度遍历 - 计算两个的节点加法所得值 - 节点存储(sum % 10),节点进位(sum / 10)
        最后考虑进位标志是否为1在尾部添加节点
    方法2：
        边遍历两个链表边作加法 - 同样方法计算两个节点所得值
        while(p != nullptr || q != nullptr) - 此循环要特别注意循环体之中得p/q空与非空得处理方法
        最后考虑进位标志是否为1在尾部添加节点
*/

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) 
    {
        ListNode* head = new ListNode(-1); //创建一个头节点哨兵

        ListNode* tail = head;             //移动指针始终指向尾部节点 - 初始为head
        int flag = 0;                      //进1标志位初始化为0

        ListNode* p = l1, *q = l2;

        while(p != nullptr || q != nullptr)
        {
            //每次x,y根据当前节点是否为空来界定
            int x = (p == nullptr) ? 0 : p->val;
            int y = (q == nullptr) ? 0 : q->val;

            tail->next = new ListNode( add(x, y, flag) );//新节点插入结果链表的尾部
            tail = tail->next;

            if(p != nullptr) //不为空才向后遍历 - 为空则当前计算数为0
                p = p->next;
            if(q != nullptr)
                q = q->next;
        }//O(max(l1->size(), l2->size())) - 线性时间

        if(flag == 1)        //特别注意循环结束后的进位标志 - 若为1需在尾部构造此节点
            tail->next = new ListNode(flag);

        return head->next;
    }

    int add(int x, int y, int& flag)    //此函数只来计算加法及是否要进位
    {
        int sum = x + y + flag;

        flag = sum / 10;    //进位值
        sum = sum % 10;     //最终存储值

        return sum;
    }
};