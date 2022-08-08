#ifndef _KARL_STACK_
#define _KARL_STACK_

#include "error.hpp"

//这里是Stack(不止当栈使,还当可变数组用awa)的实现

//由于模板的尿性,只能在模板类定义的库文件内写代码,否则实例化后会出现报错:undefined
//还有一种解决方法就是在方法实现的cpp文件最后加上 实例化(如template class Stack<Value>)
//但是那样不方便,于是就专门写在了这里,由vm.hpp包含

#define DEFAULT_SIZE 16

template <class T>
class Stack
{
private:
    T *data = nullptr; // data                 数据数组
    uint32_t cap = 0;  // capacity             总共容量
    uint32_t us = 0;   // usedsize             已使用容量
    bool pfo = false;  // pointerFromOutside   数据指针是否来自外部(为true则千万不要扩容!)

public:
    // 构造函数
    Stack(const Stack<T> &stack, uint32_t start);
    Stack(uint32_t _cap);
    Stack();

    // 析构函数
    ~Stack();

    // 初始化函数
    void init(const Stack<T> &stack, uint32_t start);
    void init(uint32_t _cap);

    // 重置函数
    void reset(T *dataPtr, uint32_t size, bool ispfo = false);

    // 清理函数
    void clearData();

    // getter
    T *getData();
    uint32_t getCapacity();
    uint32_t getUsedsize();
    bool ispfo(); // get pfo

    // 方法
    bool resize(uint32_t newCap);

    bool push(const T &element);
    T &pop();

    T *lastUsed();   // 获取最后一个被使用(不是弹出)
    T *firstEmpty(); // 获取第一个空位的地址,效果同push

    uint32_t search(const T &element);
    uint32_t registerData(T &element); // 压入数据返回索引,如果stack内存在相同数据则返回该数据索引

    // 操作符重载
    T &operator[](uint32_t idx);
};

template <class T>
Stack<T>::Stack(const Stack<T> &stack, uint32_t start)
{
    init(stack, start);
}

template <class T>
Stack<T>::Stack(uint32_t _cap)
{
    init(_cap);
}

template <class T>
Stack<T>::Stack()
{
    init(DEFAULT_SIZE);
}

template <class T>
Stack<T>::~Stack()
{
    clearData();
}

template <class T>
void Stack<T>::init(const Stack<T> &stack, uint32_t start) // start是从何处开始
{
    data = stack.data + start;
    cap = stack.cap - start;
    us = 0 <= stack.us - start ? stack.us - start : 0;
    pfo = true;
}

template <class T>
void Stack<T>::init(uint32_t _cap)
{
    data = new T[_cap];
    cap = _cap;
}

template <class T>
void Stack<T>::clearData()
{
    if (pfo || data == nullptr)
    {
        return;
    }
    delete[] data;
    data = nullptr;
    cap = 0;
    us = 0;
}

template <class T>
void Stack<T>::reset(T *dataPtr, uint32_t size, bool ispfo)
{
    clearData();
    if (dataPtr == nullptr || size == 0)
    {
        return;
    }
    data = dataPtr;
    cap = size;
    pfo = ispfo;
}

template <class T>
T *Stack<T>::getData()
{
    return data;
}

template <class T>
uint32_t Stack<T>::getCapacity()
{
    return cap;
}

template <class T>
uint32_t Stack<T>::getUsedsize()
{
    return us;
}

template <class T>
bool Stack<T>::ispfo()
{
    return pfo;
}

template <class T>
bool Stack<T>::resize(uint32_t newCap)
{
    if (cap == newCap) // 无需操作
    {
        return false;
    }
    if (pfo) // 外来数据指针,不能乱动
    {
        ErrorUnreachableBranch(); //无法到达的分支(内存将溢出,应该是执行器的某段代码出问题了)
        return false;
    }
    if (data == nullptr) // 数据为空则直接申请
    {
        data = new T[newCap];
        return true;
    }
    if (newCap == 0) // newCap为0则删除数据
    {
        delete[] data;
        data = nullptr;
        return true;
    }
    T *newdata = new T[newCap];                      // 申请内存
    for (uint32_t i = 0; i < newCap && i < cap; i++) // 存入数据
    {
        newdata[i] = data[i];
    }
    delete[] data;  // 删除旧数据
    data = newdata; // 归正data
    cap = newCap;   // 归正cap
    if (us > cap)   // 归正us
    {
        us = cap;
    }
    return true;
}

template <class T>
bool Stack<T>::push(const T &element)
{
    if (data == nullptr) // 空数据
    {
        if (!resize(DEFAULT_SIZE))
        {
            ErrorUnreachableBranch();
            return false;
        }
    }
    if (us == cap) // 数据已填满
    {
        if (!resize(ceilToPowerOf2(cap + 1)))
        {
            ErrorUnreachableBranch();
            return false;
        }
    }
    data[us] = element;
    us++;
    return true;
}

template <class T>
T &Stack<T>::pop()
{
    if (data == nullptr || us == 0)
    {
        reportError(ErrorId::stackHasNoData, true, nullptr); // 栈错误,栈无数据
        return *data;
    }
    us--;
    return data[us];
}

template <class T>
T *Stack<T>::firstEmpty()
{
    if (data == nullptr)
    {
        reportError(ErrorId::stackHasNoData, true, nullptr); // 栈错误,栈无数据
        return nullptr;
    }
    if (us == cap) // 数据已填满
    {
        if (!resize(ceilToPowerOf2(cap + 1)))
        {
            ErrorUnreachableBranch(); // 正常情况不会无法resize
            return data;
        }
    }
    us++;
    return &data[us - 1]; // 返回第一个空位的地址
}

template <class T>
T *Stack<T>::lastUsed()
{
    return &data[us - 1];
}

template <class T>
uint32_t Stack<T>::search(const T &element)
{
    for (uint32_t i = 0; i < us; i++)
    {
        if (data[i] == element)
        {
            return i;
        }
    }
    return us;
}

template <class T>
uint32_t Stack<T>::registerData(T &element)
{
    uint32_t res = search(element);
    if (res == us)
    {
        push(element);
    }
    return res;
}

template <class T>
T &Stack<T>::operator[](uint32_t idx)
{
    if (data == nullptr)
    {
        reportError(ErrorId::stackHasNoData, true, nullptr); // 栈错误,栈无数据
        return nullptr;
    }
    if (idx >= us)
    {
        reportError(ErrorId::stackIndexOutOfRange, true, nullptr); // 索引越界
        return data[0];
    }
    return data[idx];
}

#endif
