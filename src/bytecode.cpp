#include "bytecode.hpp"

int bytecodeEffect[] = {
#define HANDLE_BYTECODE(name, stringName, effect, operand) effect,
#include "bytecode.enum"
#undef HANDLE_BYTECODE
};

const char *bytecodeName[] = {
#define HANDLE_BYTECODE(name, stringName, effect, operand) stringName,
#include "bytecode.enum"
#undef HANDLE_BYTECODE
};

int bytecodeOperand[] = {
#define HANDLE_BYTECODE(name, stringName, effect, operand) operand,
#include "bytecode.enum"
#undef HANDLE_BYTECODE
};

void Instream::push(Bytecode type)
{
    data->push((byte)type);
}

void Instream::push(byte operand)
{
    data->push(operand);
}

void Instream::push(Bytecode type, byte operand)
{
    push(type);
    push(operand);
}

void Instream::push(int32_t operand, uint8_t length)
{
    switch (length)
    {
    case 1:
        data->push(operand);
        return;
    case 2:
        data->push(operand & 0xff);
        data->push(operand >> 8);
        return;
    case 4:
        data->push(operand & 0xff);
        data->push((operand >> 8) & 0xff);
        data->push((operand >> 16) & 0xff);
        data->push(operand >> 24);
        return;
    default:
        ErrorUnreachableBranch();
    }
}

void Instream::push(Bytecode type, int32_t operand, uint8_t length)
{
    push(type);
    push(operand, length);
}

int32_t Instream::pop(byte *ip)
{
    switch (bytecodeOperand[*ip])
    {
    case 1:
        return *(ip + 1);
    case 2:
        return *(int16_t *)(ip + 1);
    case 4:
        return *(int32_t *)(ip + 1);
    default:
        ErrorUnreachableBranch();
    }
    return 0;
}

int32_t Instream::pop(byte *ip, uint8_t length)
{
    switch (length)
    {
    case 1:
        return *ip;
    case 2:
        return *(int16_t *)ip;
    case 4:
        return *(int32_t *)ip;
    default:
        ErrorUnreachableBranch();
    }
    return 0;
}

void Instream::print()
{
    byte *ip = data->getData();
    Bytecode type = (Bytecode)*ip;
    while (type != Bytecode::END)
    {
        printf("%d ", *ip);
        type = (Bytecode) * ++ip;
    }
}
