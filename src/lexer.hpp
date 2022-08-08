#ifndef _KARL_LEXER_
#define _KARL_LEXER_
#include "token.hpp"
#include "utils.hpp"

class Lexer
{
public:
    // 公共成员
    String fileName;                  // 文件名
    const char *sourceCode = nullptr; // 源码
    char *nextCharPtr = nullptr;      // 下一个字符的指针
    char curChar = 0;                 // 当前字符

    Token preToken;    // 上一位token (previousToken)
    Token curToken;    // 当前位token (currentToken)
    Token nextToken;   // 下一位token (nextToken)
    Token secondToken; // 下二位token (secondToken)
    Token thirdToken;  // 下三位token (thirdToken)

    Position pos; // 当前读取到的位置

    // 构造函数
    Lexer(const String &_flieName, const char *_sourceCode);
    Lexer(const String &_flieName);
    Lexer();

    // 析构函数
    ~Lexer();

    // 初始化函数
    void init(const String &_flieName, const char *_sourceCode);
    void init(const String &_flieName);

    // 方法
    void advance();

private:
    // 隐含方法
    void init();
    void readFile();

    void skipBlanks();
    void skipAlineComment();
    void skipBlockComment();

    void setTokenPosition(); // 将当前位置放入Token

    void getNextChar();
    bool matchNextChar(char match);

    TokenType idOrKeyword(String &str);

    void parseNum();
    void parseString();
    void parseId(TokenType type);
};

#endif
