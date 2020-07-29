#include <iostream>
#include <pthread.h>

template<typename T>
class SmartPtr
{
private:
    
    T * _ptr;
    size_t refer_count;
    static pthread_mutex_t sp_mutex;
public:
    
    SmartPtr(T * p = nullptr) : _ptr(p), refer_count(0)
    {
        if(this->_ptr)
            ++refer_count;
    }
    
    SmartPtr(const SmartPtr<T>& that) //拷贝构造
    {
        if(*this != that)
        {
            this->_ptr = that._ptr;
            pthread_mutex_lock(&sp_mutex);
            this->refer_count = that.refer_count;
            this->refer_count++;
            pthread_mutex_unlock(&sp_mutex);
        }
    }
    
    SmartPtr& operator=(const SmartPtr<T>& that)
    {
        if(this->_ptr == that._ptr) //自己=自己
            return *this;
        
        if(this->_ptr == nullptr) //当前指针对象未指向任何对象
        {

        }
        else //涉及引用计数的变化
        {
            

        }
    }

    T& operator*()
    {
        if(this->_ptr)
            return *this->_ptr;
    }

    T* operator->()//返回指针
    {
        if(this->_ptr)
            return this->_ptr;
    }

    ~SmartPtr()
    {
        this->refer_count--;
        if(refer_count == 0)
        {
            delete _ptr;
        }

    }
    
    size_t use_count()
    {
        return refer_count;
    }
};

template<typename T>
pthread_mutex_t SmartPtr<T>::sp_mutex = PTHREAD_MUTEX_INITIALIZER;


int main()
{





    return 0;
}

