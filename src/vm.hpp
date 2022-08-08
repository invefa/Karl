#ifndef _KARL_VM_
#define _KARL_VM_
#include "basic.hpp"

class Frame
{
public:
    uint32_t ip;    // 当前位置
    byte *instream; // 字节流
    Frame *upFrame; // 调用者
};

class Module // 模块,即文件被编译后的结果
{
public:
    String name;                // 模块名称
    Stack<Value> constants = 1; // 模块的常量
    Stack<Value> varlist = 1;   // 模块的全局变量表
    Stack<Fn> fnlist = 1;       // 模块的函数表
};

class VM
{
public:
    Stack<Module> modules = 1; // 运行所需的所有模块
    Stack<Value> varlist = 8;  // 运行时变量存点
    Stack<Value> stack = 16;   // 运行总栈
    Value reg[16];             // 寄存器(目前只提供16个,够用就行)
    Frame *frame;              // 当前运行框架
};

#endif
