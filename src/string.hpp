#ifndef _KARL_STRING_
#define _KARL_STRING_
#include <stdio.h>
#include <stdint.h>

// 语言内的String转化为此对象处理
class String
{
private:
    char *data = nullptr;
    uint32_t length = 0;
    bool constData = false; //为true则不会析构,因为const char*的data是常量文本无法释放

    // 隐藏函数
    void copy(const String &source, String &target); // 直接复制到target,且自动清理target的数据

public:
    // 构造函数(使用init函数)
    String(char *_data, uint32_t _length); // 浅复制,析构时不会释放data
    String(const char *_data);             // 浅复制,析构时不会释放data
    String(const String &str);             // 浅复制,析构时不会释放data
    String(char *_data);                   // 浅复制,data的释放权由this掌管
    String();                              // 空数据(nullptr)

    //析构函数
    ~String();

    // 初始化函数
    void init(char *_data, uint32_t _length); // 浅复制,析构时不会释放data
    void init(const char *_data);             // 浅复制,析构时不会释放data
    void init(const String &str);             // 浅复制,析构时不会释放data
    void init(char *_data);                   // 浅复制,data的释放权由this掌管

    // getter
    uint32_t getLength();
    char *getData();
    bool isConstData();

    // setter
    void setLength(uint32_t _length); // 设置长度不会影响constData变量
    void setData(char *_data);        // set的data不会在析构时释放
    void setIsConstData(bool is);     // 除非万不得已,最好别用

    // 深复制函数(抄一个一模一样的当数据,会自行释放数据)
    void deepCopy(const String &str);
    String deepCopy();

    // 浅复制函数(直接拿来当数据,且不会释放数据)
    void shallowCopy(const String &str);
    String shallowCopy();

    // 清除函数(删除所有数据,且置为空)
    void clearData();

    // 操作符重载
    String &operator=(String &str);          // 属于浅复制,并且str.data的释放权会被交给this
    String &operator=(const char *_data);    // 直接收录_data指针为数据,且析构时不会释放data
    String &operator=(char *_data);          // 直接收录_data指针为数据
    bool operator==(const char *cstr);       // 字符串是否相等
    bool operator==(const String &another);  // 字符串是否相等
    String operator+(const char *cstr);      // 与charStr拼接
    String operator+(const String &another); // 与String拼接
    String operator*(uint32_t count);        // 将文本复制多次
};

void printf(String str);
void printf(String *str);

#endif
