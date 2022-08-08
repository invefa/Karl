#include "token.hpp"
#include "error.hpp"

const char *TokenTypeName[] = {
#define HANDLE_SIGNTOKEN(name, stringName) stringName,
#define HANDLE_KWTOKEN(name, stringName) stringName,
#define HANDLE_TKWTOKEN(name, stringName) stringName,
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
};

const char *KeywordTokenName[] = {
#define HANDLE_SIGNTOKEN(name, stringName)
#define HANDLE_KWTOKEN(name, stringName) stringName,
#define HANDLE_TKWTOKEN(name, stringName) stringName,
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
};

TokenType TypeKeywordTokenType[] = {
#define HANDLE_SIGNTOKEN(name, stringName)
#define HANDLE_KWTOKEN(name, stringName)
#define HANDLE_TKWTOKEN(name, stringName) TokenType::name,
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
};

// 关键词TokenType总数(凑合用吧awa)
uint32_t KeywordTokenTypeTotal = 0
#define HANDLE_SIGNTOKEN(name, stringName)
#define HANDLE_KWTOKEN(name, stringName) +1
#define HANDLE_TKWTOKEN(name, stringName) +1
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
    ; // 这个分号可别删了哈

// 类型关键词TokenType总数
uint32_t TypeKeywordTokenTypeTotal = 0
#define HANDLE_SIGNTOKEN(name, stringName)
#define HANDLE_KWTOKEN(name, stringName)
#define HANDLE_TKWTOKEN(name, stringName) +1
#include "token.enum"
#undef HANDLE_SIGNTOKEN
#undef HANDLE_KWTOKEN
#undef HANDLE_TKWTOKEN
    ; // 这个分号可别删了哈

Token::Token(TokenType _type, char *start, uint32_t length)
{
    init(_type, start, length);
}

Token::Token(TokenType _type)
{
    type = _type;
}

void Token::init(TokenType _type, char *start, uint32_t length)
{
    type = _type;
    extract.init(start, length);
}

Token::Token()
{
    type = TokenType::unknown;
}

void Token::operator=(const Token &another)
{
    type = another.type;
    extract.shallowCopy(another.extract); // 这里的赋值是浅复制,特意设计的
    value = another.value;
    pos = another.pos;
}

TokenType isKeyword(String &str)
{
    for (uint32_t i = 0; i < KeywordTokenTypeTotal; i++)
    {
        if (str == String(KeywordTokenName[i]))
        {
            return (TokenType)i;
        }
    }
    return TokenType::unknown;
}

bool isTypeKeyword(TokenType type)
{
    for (uint32_t i = 0; i < TypeKeywordTokenTypeTotal; i++)
    {
        if (type == TypeKeywordTokenType[i])
        {
            return true;
        }
    }
    return false;
}

ValueType getCorTokenType(TokenType type)
{
    switch (type)
    {
    case TokenType::int_:
        return ValueType::Int;
    case TokenType::string_:
        return ValueType::String;
    case TokenType::long_:
        return ValueType::Long;
    case TokenType::double_:
        return ValueType::Double;
    default:
        ErrorUnreachableBranch();
    }
    return ValueType::Unknown;
}
