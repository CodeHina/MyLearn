#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

/** LRU - 失败版本 - 利用了链表的删除操作 - 复杂度过大
class LRUCache {

private:

    list<int>* plist;
    unordered_map<int, int >* phash_map;
    int max_size;
    int cur_size;

public:
    LRUCache(int capacity) : max_size(capacity), cur_size(0)
    {
        plist = new list<int>;
        phash_map = new unordered_map<int, int>;
    }

    int get(int key)
    {
        auto iter = phash_map->find(key);
        if( iter == phash_map->end() )
            return -1;
        else    //相当于一次访问 - 故需将访问的数据更新到链表头
        {
            plist->remove(key);
            plist->push_front(key); //先删除当前元素再插入至链表头 - 表示刚刚访问了此元素
            return iter->second;
        }


    }

    void put(int key, int value)
    {
        if(cur_size == max_size) //当前缓存已满
        {
            if(get(key) == -1) //当前元素不存在
            {
                int temp_k = -1;
                if(!plist->empty())
                    temp_k = plist->back();//备份最近最少使用的值的key

                plist->pop_back();//删除尾部元素

                phash_map->erase(temp_k);//删除此键值对

                plist->push_front(key);
                phash_map->insert( {key,value} );
            }
            else
            {
                plist->remove(key);
                plist->push_front(key); //先删除当前元素再插入至链表头 - 表示刚刚访问了此元素
                phash_map->find(key)->second = value;//更新值
            }
        }
        else //缓存未满
        {
            if(get(key) == -1) //且当前元素不存在
            {
                phash_map->insert( {key, value} ); //构造键值对放入哈希表
                plist->push_front(key);            //相当于对元素的访问 - 故放置在链表头表示刚刚访问

                ++cur_size; //更新缓存
            }
            else //当前元素存在
            {
                plist->remove(key);     //这里利用了键的唯一性 - 但是时间复杂度很大 - 不能达到O(1)
                plist->push_front(key); //先删除当前元素再插入至链表头 - 表示刚刚访问了此元素
                phash_map->find(key)->second = value;//更新值
            }
        }
    }
};
**/

struct node
{
    int m_key;
    int m_value;

    node* pre;  //前驱
    node* suc;  //后继

    node(int key, int value, node* p = nullptr, node* s = nullptr) : m_key(key), m_value(value), pre(p), suc(s) { }

    node* insert_as_pre(const int& key, const int& value) //在当前节点插入一个前驱节点
    {
        node* new_node = new node(key, value, this->pre, this);
        this->pre->suc = new_node;
        this->pre = new_node;

        return new_node;
    }

    node* insert_as_suc(const int& key, const int& value)
    {
        node* new_node = new node(key, value, this, this->suc);
        this->suc->pre = new_node;
        this->suc = new_node;

        return new_node;
    }

};

struct dlist
{
    int m_size;
    node* head;
    node* tail;

    dlist() : m_size(0)             //构造空链表
    {
        head = new node(-1, -1);
        tail = new node(-1, -1);
        head->suc = tail;
        tail->pre = head;
    }

    node* insert_as_first(int key, int value)
    {
        m_size++;
        return head->insert_as_suc(key ,value);
    }

    node* insert_as_lase(int key, int value)
    {
        m_size++;
        return tail->insert_as_pre(key, value);
    }

    void remove_node(node* cur)
    {
        cur->pre->suc = cur->suc;
        cur->suc->pre = cur->pre;

        delete cur;
        --m_size;
    }
};


class LRUCache
{
/**
    遇到问题先分析 - LRU缓存机制是一种常用的页面置换算法
    将最近最少使用的元素替换出缓存中(删除) - 删除快速的数据结构链表O(1)
    访问也要快速 - 访问快速的结构 - 数组、哈希表
    键值对 - 哈希表
    能够根据访问顺序区分刚刚访问的和最近一直没有访问的 - 双向链表 - 表头为刚刚访问的,表尾为最近一直没有访问的

    故采用双向链表和哈希表

    访问表内已经存在的数据时,需要此元素更新到表头
    若采用删除链表的元素,需要先找到此元素地址即指向该节点的指针 - 若采用链表查找的方法,复杂度O(N)
    故考虑将此链表节点指针和key值一一对应构造健值对 pair(int, node*);
    而链表存放key - value

**/

private:

    dlist my_list;
    unordered_map<int, node*> my_hashMap;
    int max_size;

public:
    LRUCache(int capacity)
    {
        max_size = capacity;    //最大缓存元素个数
    }

    int get(int key)
    {
        if( my_hashMap.find(key) !=  my_hashMap.end() )//当前节点存在
        {

            node* wait_del = my_hashMap.find(key)->second;      //从哈希表中取出此key对应的节点指针
            int temp_val = wait_del->m_value;
            my_list.remove_node(wait_del);                      //删除此节点

            node* p = my_list.insert_as_first(key, temp_val);   //作为首节点插入即可 - 表示刚刚访问过
            my_hashMap[key] = p;                                //同时更新添加键值对 - 无需更新值

            return temp_val;
        }
        return -1;
    }

    void put(int key, int value)
    {
        if(get(key) == -1)//节点不存在
        {
            if(my_list.m_size == max_size)//且缓存满了
            {
                my_hashMap.erase(my_list.tail->pre->m_key); //删除尾部节点键值对
                my_list.remove_node(my_list.tail->pre);   //摘除尾部节点

                node* p = my_list.insert_as_first(key, value);    //作为首节点插入即可 - 表示刚刚访问过
                my_hashMap[key] = p;                              //同时更新添加键值对

            }
            else//缓存未满
            {
                node* p = my_list.insert_as_first(key, value);    //作为首节点插入即可 - 表示刚刚访问过
                my_hashMap[key] = p;                              //同时更新添加键值对
            }
        }
        else//节点存在 - 不涉及缓存问题 - 在于更新节点的位置和值
        {
            node* wait_del = my_hashMap.find(key)->second;      //从哈希表中取出此key对应的节点指针
            my_list.remove_node(wait_del);                      //删除此节点

            node* p = my_list.insert_as_first(key, value);      //作为首节点插入即可 - 表示刚刚访问过
            my_hashMap[key] = p;                                //同时更新添加键值对
        }

    }
};



int main()
{
	LRUCache cache(2);

    cache.put(2,1);
    cache.put(2,2);
    std::cout << cache.get(2) << "\n";  //2
    cache.put(1,1);
    cache.put(4,1);
    std::cout << cache.get(2);  //-1




}









