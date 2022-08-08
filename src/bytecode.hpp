#ifndef _KARL_BYTECODE_
#define _KARL_BYTECODE_
#include "utils.hpp"
#include "stack.hpp"
#include <stdint.h>

enum class Bytecode : uint8_t
{
#define HANDLE_BYTECODE(name, stringName, effect, operand) name,
#include "bytecode.enum"
#undef HANDLE_BYTECODE
};

extern const char *bytecodeName[];
extern int bytecodeEffect[];
extern int bytecodeOperand[];

// 只是封装一下,方便用函数
class Instream
{
public:
    Stack<byte> *data;

    void push(Bytecode type);

    void push(byte operand);
    void push(Bytecode type, byte operand);

    void push(int32_t operand, uint8_t length);
    void push(Bytecode type, int32_t operand, uint8_t length);

    int32_t pop(byte *ip);
    int32_t pop(byte *ip, uint8_t length);

    void print(); // 输出当前字节流
};

#endif
