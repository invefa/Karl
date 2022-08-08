#include "string.hpp"

String::String(char *_data, uint32_t _length)
{
    init(_data, _length);
}

String::String(const char *_data)
{
    init(_data);
}

String::String(char *_data)
{
    init(_data);
}

String::String(const String &str)
{
    init(str);
}

String::String()
{
    data = nullptr;
    length = 0;
    constData = false;
}

String::~String()
{
    clearData();
}

void String::init(char *_data, uint32_t _length)
{
    if (_data == nullptr)
    {
        clearData();
        return;
    }
    data = _data;
    length = _length;
    constData = true;
}

void String::init(const char *_data)
{
    if (_data == nullptr)
    {
        clearData();
        return;
    }
    data = (char *)_data;
    length = 0;
    while (_data[length] != '\0')
    {
        length++;
    }
    constData = true;
}

void String::init(char *_data)
{
    if (_data == nullptr)
    {
        clearData();
        return;
    }
    data = _data;
    length = 0;
    while (_data[length] != '\0')
    {
        length++;
    }
    constData = false;
}

void String::init(const String &str)
{
    data = str.data;
    length = str.length;
    constData = true;
}

void String::clearData()
{
    if (data != nullptr && !constData)
    {
        delete[] data;
    }
    data = nullptr;
    length = 0;
    constData = false;
}

void String::copy(const String &source, String &target)
{
    target.clearData();
    if (source.data == nullptr) // 源为空则置为空,clearData会自动置空,这里只需返回
    {
        return;
    }
    target.data = new char[source.length + 1];
    for (uint32_t i = 0; i < source.length; i++) // 赋值
    {
        target.data[i] = source.data[i];
    }
    target.data[source.length] = '\0'; // 设置最后一位,防止内存溢出
    target.length = source.length;     // 归正length
}

String String::deepCopy()
{
    String str;
    copy(*this, str); // 把this的数据复制到str
    return str;
}

void String::deepCopy(const String &str)
{
    copy(str, *this); // 把str的数据复制到this
}

String String::shallowCopy()
{
    String str;
    clearData();
    str.init(*this);
    return str;
}

void String::shallowCopy(const String &str)
{
    clearData();
    init(str);
}

String &String::operator=(String &str)
{
    clearData();
    if (str.data == nullptr)
    {
        return *this;
    }
    data = str.data;
    length = str.length;
    if (!str.constData)
    { // 如果str的data不是constData,str.data的释放权就交给this
        str.constData = true;
        constData = false;
        return *this;
    }
    constData = true;
    return *this;
}

String &String::operator=(const char *_data)
{
    clearData();
    init(_data);
    return *this;
}

String &String::operator=(char *_data)
{
    clearData();
    init(_data);
    return *this;
}

bool String::operator==(const char *cstr)
{
    String str = cstr;
    return *this == str; // .*this == str 也就是调用了下面这个函数
}

bool String::operator==(const String &another)
{
    if (another.data == nullptr || data == nullptr || length != another.length)
    {
        return false;
    }
    if (data == another.data) // 地址相同,且长度相同
    {
        return true;
    }
    for (uint32_t i = 0; i < length; i++)
    {
        if (data[i] != another.data[i]) // 文本数据不同
        {
            return false;
        }
    }
    return true;
}

String String::operator+(const char *cstr)
{
    String str = cstr;
    return *this + str;
}

String String::operator+(const String &another)
{
    String str;
    str.length = length + another.length; // 在String类里可以直接改String变量str里的private成员
    str.data = new char[str.length + 1];
    for (uint32_t i = 0; i < length; i++) // 将this.data值赋值到str.data的上半部分
    {
        str.data[i] = data[i];
    }
    for (uint32_t i = 0; i < another.length; i++) // 将another.data值赋值到str.data的下半部分
    {
        str.data[length + i] = another.data[i];
    }
    str.data[str.length] = '\0'; // 设置最后一位,防止内存溢出
    return str;
}

String String::operator*(uint32_t count)
{
    String res;
    res.length = length * count; // length赋值写在前面,省了一个临时变量
    res.data = new char[res.length + 1];
    for (uint32_t i = 0; i < res.length; i++) // 复制字符串count次
    {
        res.data[i] = data[i % length];
    }
    res.data[res.length] = '\0'; // 设置最后一位,防止内存溢出
    return res;
}

uint32_t String::getLength()
{
    return length;
}

char *String::getData()
{
    return data;
}

bool String::isConstData()
{
    return constData;
}

void String::setLength(uint32_t _length)
{
    length = _length;
}

void String::setData(char *_data)
{
    data = _data;
    constData = true;
}

void String::setIsConstData(bool is)
{
    constData = is;
}

void printf(String str)
{
    printf(str.getData());
}

void printf(String *str)
{
    printf(str->getData());
}
