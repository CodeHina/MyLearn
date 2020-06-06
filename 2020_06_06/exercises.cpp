/*

    //1.栈通常使用的两种数据结构为 - 顺序存储结构和链式存储结构
        
        存储方式分为两种 - 顺序存储和链式存储(计算机角度)

    //2.二叉树不同度节点的一般规律
        
        叶子节点(n0) = 度为2节点(n2) + 1

    //3.内联函数
        
        inline 关键字修饰的函数 - 由编译器决定是否内联
        引入内联函数是避免进程调用函数时堆栈的内存的大量消耗

        内联函数会在编译期间在进行替换 - 并且像普通函数一样进行类型检查
        宏函数只是在预处理阶段进行简单的替换 - 并不会进行类型检查

        并且调试阶段可以对内联函数进行调试 - 而宏函数是不可以的

        并且类内部定义的函数自动声明为内联函数

    //4.m阶B树 - m路平衡搜索树
    
        分级存储结构中，相邻存储级别间的数据传输 - I/O操作 - 由于不同存储结构地访问速度不同 - 应该尽可能地减少I/O操作
        我们更希望与增加内存的访问 - 而减少对外存的访问
        
        于是将平衡二叉树的节点进行合并 - 将多个小节点合并为一个大节点 - 每次从外存中一次读入多个节点

        合并策略：如以2层为间隔进行合并 - 每个大节点最多 3个小节点 4个分支
                  如以3层为间隔进行合并 - 每个大节点最多 7个小节点 8个分支
        
        由此m阶B树 - 大节点包含最多不超过m-1个小节点,m个分支
        同样节点也不能太少 - 除根节点外,其小节点的个数满足 n + 1 >= m/2的上整
                             根节点满足：n + 1 >= 2
        故其分支数介于m/2的上整和m之间 - m阶B树也被称为[m/2,m]树

    //5.哈夫曼树
    
        哈夫曼树的一个特点就是只有度为0和度为2的节点 - 无度为1的节点

        建立一棵哈夫曼树的时间复杂度为O(nlogn)

        哈夫曼编码

    //6.fork()函数
        
        for(int i = 0; i < n; ++i)
            fork();

        由于子进程的逻辑和父进程相同 - 故会共有2^(n+1)个进程
    
    //7.new/delete 和 malloc/free
        
        new/delete 是运算符 - 在类内能够重载此运算符(成员函数)

        new是会先调用operator new()函数,分配足够内存(底层malloc) - 再调用类的构造函数
        delete是先调用类的析构函数,再调用operator delete(),释放内存(底层free)
*/

