#ifndef _KARL_FN_
#define _KARL_FN_
#include "utils.hpp"
#include "value.hpp"
#include "stack.hpp"

// 函数签名(参数和返回值的类型等信息)
class Signature
{
public:
    ValueType ret = ValueType::Unknown; // 返回值类型
    uint8_t argCount = 0;               // 参数个数
    ValueType *argsType = nullptr;      // 参数的类型

    // 设置函数
    void set(ValueType _ret, uint8_t _argCount, ValueType *_argsType);

    // 功能函数
    void addArg(ValueType type);
};

union FnData
{
    primitive prim = nullptr;
    Stack<byte> *instream;
};

class Fn
{
public:
    String name;       // 函数名(运行期可空)
    bool prim = false; // 是否为原生函数(c/c++函数)
    FnData data;       // 函数数据,原生或字节码
    Signature sign;    // 函数签名

    // 构造函数
    Fn(String &_name);
    Fn();

    // 设置函数
    void set(Stack<byte> *_ins, uint8_t _argCount);
    void set(primitive _prim, uint8_t _argCount);
};

#endif
