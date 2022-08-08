#include "basic.hpp"

Fn::Fn(String &_name)
{
    name.deepCopy(_name);
}

Fn::Fn()
{
}

void Fn::set(Stack<byte> *_ins, uint8_t _argCount)
{
    prim = false;
    data.instream = _ins;
    sign.argCount = _argCount;
}

void Fn::set(primitive _prim, uint8_t _argCount)
{
    prim = true;
    data.prim = _prim;
    sign.argCount = _argCount;
}

void Signature::set(ValueType _ret, uint8_t _argCount, ValueType *_argsType)
{
    ret = _ret;
    argCount = _argCount;
    argsType = _argsType;
}

void Signature::addArg(ValueType type)
{
    if (argsType == nullptr)
    {
        argsType = new ValueType(type);
        argCount++;
        return;
    }
    ValueType *newdata = new ValueType[argCount + 1]; // 新增一位
    for (uint8_t i = 0; i < argCount; i++)
    { // 遍历储存
        newdata[i] = argsType[i];
    }
    newdata[argCount] = type; // 设置最后一位
    argCount++;               // 归正参数个数
    delete[] argsType;        // 删除旧的
    newdata = argsType;       // 放入新的
}
