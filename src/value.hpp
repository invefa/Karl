#ifndef _KARL_BASIC_
#define _KARL_BASIC_
#include "string.hpp"

// 语言内的Value类型
enum class ValueType : uint8_t
{
#define HANDLE_VALUE(name, stringName) name,
#include "value.enum"
#undef HANDLE_VALUE
};

extern const char *ValueTypeName[];

// 各种数值类型的union
union Num
{
    char Char;
    int Int;
    long long Long;
    float Float;
    double Double;
};

// Value数据
union ValueData
{
    String *string;
    void *ptr;
    double num;
    Num base;
};

//类型系统的构造
class Value
{
public:
    // 公共成员
    ValueType type;
    ValueData data;

    // 构造函数
    Value(ValueType _type, ValueData _data);
    Value(const char *_string);
    Value(ValueType _type);
    Value(String *_string); //使用传递的String为值
    Value(String _string);  //新建String为值,data不动
    Value(double _num);
    Value(void *_ptr);
    Value();

    // 析构函数
    ~Value();

    // 清理函数
    void clearData();

    // 赋值运算
    const Value &operator=(const Value &another);

    // 逻辑运算
    bool operator||(const Value &another);
    bool operator&&(const Value &another);
    bool operator==(const Value &another);
    bool operator>=(const Value &another);
    bool operator<=(const Value &another);
    bool operator>(const Value &another);
    bool operator<(const Value &another);

    // 数学运算(目前只有加法支持字串符操作其他都只能数值运算)
    Value operator+(const Value &another);
    Value operator-(const Value &another);
    Value operator*(const Value &another);
    Value operator/(const Value &another);
    Value operator%(const Value &another);

    // 调试使用
    void print(bool fullPrint = false);
};

inline bool isSameType(Value a, Value b)
{
    return a.type == b.type;
}

#endif
