#include "value.hpp"
#include <stdio.h>
#include "error.hpp"

const char *ValueTypeName[] = {
#define HANDLE_VALUE(name, stringName) stringName,
#include "value.enum"
#undef HANDLE_VALUE
};

Value::Value(ValueType _type, ValueData _data)
{
    type = _type;
    data = _data;
}

Value::Value(ValueType _type)
{
    type = _type;
    data.ptr = nullptr;
}

Value::Value(String *_string)
{
    type = ValueType::String;
    data.string = _string;
}

Value::Value(String _string)
{
    type = ValueType::String;
    data.string = new String(_string.getData(), _string.getLength());
}

Value::Value(const char *_string)
{
    type = ValueType::String;
    data.string = new String(_string);
}

Value::Value(void *_ptr)
{
    type = ValueType::Ptr;
    data.ptr = _ptr;
}

Value::Value(double _num)
{
    type = ValueType::Num;
    data.num = _num;
}

Value::Value()
{
    type = ValueType::Unknown;
    data.ptr = nullptr;
}

Value::~Value()
{
    clearData();
}

void Value::clearData()
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Num:
        break;
    case ValueType::String:
        if (data.string == nullptr)
        {
            return;
        }
        delete data.string;
        data.string = nullptr;
        break;
    case ValueType::Ptr:
        ErrorUnreachableBranch();
        break;
    }
}

#define ErrorIsNotSameType()                                     \
    reportError(ErrorId::valueTypeDissimilarity, false, nullptr, \
                ValueTypeName[(uint32_t)this->type],             \
                ValueTypeName[(uint32_t)another.type]);

const Value &Value::operator=(const Value &another)
{
    clearData();
    type = another.type;
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
        break;
    case ValueType::Num:
        data.num = another.data.num;
        break;
    case ValueType::String:
        data.string->deepCopy(*another.data.string);
        break;
    case ValueType::Ptr:
        data.ptr = another.data.ptr;
        break;
    }
    return another;
}

bool Value::operator==(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
        return true;
    case ValueType::Num:
        return data.num == another.data.num;
    case ValueType::String:
        return &data.string == &another.data.string;
    case ValueType::Ptr:
        return data.ptr == another.data.ptr;
    }
    return false;
}

bool Value::operator>=(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::Num:
        return data.num >= another.data.num;
    }
    return false;
}

bool Value::operator<=(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::Num:
        return data.num <= another.data.num;
    }
    return false;
}

bool Value::operator>(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::Num:
        return data.num > another.data.num;
    }
    return false;
}

bool Value::operator<(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::Num:
        return data.num < another.data.num;
    }
    return false;
}

bool Value::operator||(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::True:
        return true;
    case ValueType::False:
        if (another.type == ValueType::True)
        {
            return true;
        }
    }
    return false;
}

bool Value::operator&&(const Value &another)
{
    if (!isSameType(*this, another))
    {
        ErrorIsNotSameType();
        return false;
    }
    switch (type)
    {
    case ValueType::False:
        return false;
    case ValueType::True:
        if (another.type == ValueType::False)
        {
            return false;
        }
    }
    return true;
}

Value Value::operator+(const Value &another)
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Ptr:
        ErrorIsNotSameType();
        return Value();
    case ValueType::Num:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value(); //暂时不支持:1 + "str" -> "1str"
        }
        return Value(data.num + another.data.num);
    case ValueType::String:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value(); //暂时不支持:"str" + 1 -> "str1"
        }
        return Value(*data.string + *another.data.string);
    }
    ErrorUnreachableBranch();
    return Value();
}

Value Value::operator-(const Value &another)
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Ptr:
    case ValueType::String:
        ErrorIsNotSameType();
        return Value();
    case ValueType::Num:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value();
        }
        return Value(data.num - another.data.num);
    }
    ErrorUnreachableBranch();
    return Value();
}

Value Value::operator*(const Value &another)
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Ptr:
    case ValueType::String:
        ErrorIsNotSameType();
        return Value();
    case ValueType::Num:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value();
        }
        return Value(data.num * another.data.num);
    }
    ErrorUnreachableBranch();
    return Value();
}

Value Value::operator/(const Value &another)
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Ptr:
    case ValueType::String:
        ErrorIsNotSameType();
        return Value();
    case ValueType::Num:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value();
        }
        return Value(data.num / another.data.num);
    }
    ErrorUnreachableBranch();
    return Value();
}

Value Value::operator%(const Value &another)
{
    switch (type)
    {
    case ValueType::Unknown:
    case ValueType::Null:
    case ValueType::Ptr:
    case ValueType::String:
        ErrorIsNotSameType();
        return Value();
    case ValueType::Num:
        if (!isSameType(*this, another))
        {
            ErrorIsNotSameType();
            return Value();
        }
        return Value((double)((long long)data.num % (long long)another.data.num)); //没办法,只能丢精度
    }
    ErrorUnreachableBranch();
    return Value();
}

void Value::print(bool fullPrint)
{
    if (fullPrint)
    {
        printf("ValueType:%s  Value:", ValueTypeName[(int)type]);
    }
    switch (type)
    {
    case ValueType::Null:
        printf("null");
        break;
    case ValueType::Num:
        printf("%lf", data.num);
        break;
    case ValueType::String:
        printf("\"%s\"", data.string->getData());
        break;
    case ValueType::Ptr:
        printf("%p", data.ptr);
        break;
    default:
        printf("Unknown");
        break;
    }
}
