#ifndef _KARL_ERROR_
#define _KARL_ERROR_
#include "string.hpp"
#include "utils.hpp"

// 错误的大致类别,是为reportError服务的
enum class ErrorType
{
#define HANDLE_ERROR(name, stringName, msg) name,
#include "error.enum" // 该文件存储具体信息
#undef HANDLE_ERROR
};

extern const char *ErrorName[];
extern const char *ErrorMsg[];

// ErrorId为ErrorType的分支,具体的错误信息都在这里定义
enum class ErrorId
{
#define HANDLE_ERRORID(name, stringName, belong, msg) name,
#include "errorid.enum" // 该文件存储具体信息
#undef HANDLE_ERRORID
};

extern const char *ErrorIdName[];
extern ErrorType ErrorIdBelong[];
extern const char *ErrorIdMsg[];

void reportError(ErrorId id, bool doExit, void *ptr, ...);

#define ErrorUnreachableBranch() reportError(ErrorId::unreachableBranch, true, nullptr, __FILE__, __LINE__)

// 这个是调试bug用的
#define _ErrorUnreachableBranch() reportError(ErrorId::unreachableBranch, false, nullptr, __FILE__, __LINE__)

#endif
