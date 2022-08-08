#ifndef _KARL_UTILS_
#define _KARL_UTILS_
#include <stdint.h>

class Value;

typedef int8_t byte;

// 原生函数指针
typedef Value (*primitive)(Value *);

// 好像没啥用的全局变量
extern primitive __prim__;

#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

// 给Lexer用的
class Position
{
public:
    uint32_t line = 0;
    uint32_t column = 0;
};

// // 合并与解析字节码参数用
// struct Bit16
// {
//     byte up;
//     byte down;
// };

// // 合并与解析字节码参数用
// struct Bit32
// {
//     byte up;
//     byte midup;
//     byte midown;
//     byte down;
// };

// 找出大于等于num最近的2次幂,就是与2的次数对齐
uint32_t ceilToPowerOf2(uint32_t num);

#endif
