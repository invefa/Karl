#include "error.hpp"
#include <stdlib.h>
#include <stdarg.h>
#include "lexer.hpp"
#include "compiler.hpp"

const char *ErrorName[] = {
#define HANDLE_ERROR(name, stringName, msg) stringName,
#include "error.enum"
#undef HANDLE_ERROR
};

const char *ErrorMsg[] = {
#define HANDLE_ERROR(name, stringName, msg) msg,
#include "error.enum"
#undef HANDLE_ERROR
};

const char *ErrorIdName[] = {
#define HANDLE_ERRORID(name, stringName, belong, msg) stringName,
#include "errorid.enum"
#undef HANDLE_ERRORID
};

ErrorType ErrorIdBelong[] = {
#define HANDLE_ERRORID(name, stringName, belong, msg) belong,
#include "errorid.enum"
#undef HANDLE_ERRORID
};

const char *ErrorIdMsg[] = {
#define HANDLE_ERRORID(name, stringName, belong, msg) msg,
#include "errorid.enum"
#undef HANDLE_ERRORID
};

void reportError(ErrorId id, bool doExit, void *ptr, ...)
{
    // 这里是可变参数的灵魂awa
    char buffer[512] = {'\0'};
    va_list args;
    va_start(args, ptr); //设置args为ptr之后的参数

    // 将ErrorId枚举转成整数作为数据数组的索引
    uint32_t idx = (uint32_t)id;

    // 将可变参数加入ErrorId的Msg格式中
    vsnprintf(buffer, 512, ErrorIdMsg[idx], args);

    char outbuffer[512] = {'\0'};

    // 二重格式化到ErrorMsg(以后可能会用到)
    snprintf(outbuffer, 512, ErrorMsg[(uint32_t)ErrorIdBelong[idx]], buffer);

    //判断当前ErrorId属于哪种ErrorType,并对号入座进行报错
    switch (ErrorIdBelong[idx])
    {
    case ErrorType::core:
    {
        fprintf(stderr, outbuffer);
        break;
    }
    case ErrorType::lexer:
    {
        Lexer *lexer = (Lexer *)ptr;
        if (ptr == nullptr) // 到达这个分支说明某地方的代码出了问题,输出一下方便排查
        {
            ErrorUnreachableBranch();
        }
        // lexer的错误就只能是实际处理位置的错误
        fprintf(stderr, "%s:[%d:%d]: [Lexer]error: %s", lexer->fileName.getData(),
                lexer->pos.line, lexer->pos.column, outbuffer);
        break;
    }
    case ErrorType::compiler:
    {
        if (ptr == nullptr) // 到达这个分支说明某地方的代码出了问题,输出一下方便排查
        {
            ErrorUnreachableBranch();
        }
        CompileUnit *cunit = (CompileUnit *)ptr;
        Lexer *lexer = &cunit->lexer;
        // compiler的错误定在当前token周边,于是位置为curToken的位置
        fprintf(stderr, "%s:[%d:%d]: [Compiler]error: %s", lexer->fileName.getData(),
                lexer->curToken.pos.line, lexer->curToken.pos.column, outbuffer);
        break;
    }
    }

    //清除可变参数
    va_end(args);

    //退出
    if (doExit)
    {
        exit((uint32_t)id);
    }
}
