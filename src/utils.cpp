#include "utils.hpp"

uint32_t ceilToPowerOf2(uint32_t num)
{
    num += (num == 0); //修复当num等于0时结果为0的边界情况
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;
    return num;
}
