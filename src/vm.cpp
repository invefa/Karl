#include "vm.hpp"

// void *VM::realloc(void *ptr, size_t oldSize, size_t newSize)
// {
//     usedMemory += newSize - oldSize; //累计系统分配的总内存
//     MaxUsedMemory = MAX(usedMemory, MaxUsedMemory);
//     if (newSize != 0) //避免realloc(NULL, 0)定义的新地址,此地址不能被释放
//     {
//         return realloc(ptr, newSize);
//     }

//     delete[] ptr;
//     return NULL;
// }

// void *VM::malloc(size_t size);

// void VM::free(void *ptr);
#include "vm.hpp"
