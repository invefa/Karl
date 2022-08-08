#ifndef _KARL_TOKEN_
#define _KARL_TOKEN_
#include "value.hpp"
#include "utils.hpp"

enum class TokenType : uint8_t
{
#define HANDLE_SIGNTOKEN(name, stringName) name,
#define HANDLE_KWTOKEN(name, stringName) name,
#define HANDLE_TKWTOKEN(name, stringName) name,
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
};

extern const char *TokenTypeName[];
extern const char *KeywordTokenName[];
extern TokenType TypeKeywordTokenType[];

extern uint32_t KeywordTokenTypeTotal;
extern uint32_t TypeKeywordTokenTypeTotal;

class Token
{
public:
    TokenType type = TokenType::unknown; // Token类型
    String extract;                      // Token对应文本(不能直接打印,因为data取的是source指针的,要打印就copy一下,记得回收data)
    Value value;                         // Token对应字面量的值
    Position pos;                        // token的文本位置

    //构造函数,好似没鸟用
    Token(TokenType _type, char *start, uint32_t length);
    Token(TokenType _type);
    Token();

    // 初始化函数,好似没鸟用
    void init(TokenType _type, char *start, uint32_t length);

    // 操作符重载
    void operator=(const Token &another);
};

TokenType isKeyword(String &str);
bool isTypeKeyword(TokenType type);

// 获取类型关键词token类型对应的value类型
ValueType getCorTokenType(TokenType type);

#endif
