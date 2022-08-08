#include "lexer.hpp"
#include "error.hpp"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// processed token(被处理的token)
#define PTOKEN thirdToken

Lexer::Lexer(const String &_flieName)
{
    init(_flieName);
}

Lexer::Lexer(const String &_flieName, const char *_sourceCode)
{
    init(_flieName, _sourceCode);
}

Lexer::Lexer()
{
    // null~
}

Lexer::~Lexer()
{
    if (sourceCode != nullptr)
    {
        delete[] sourceCode;
    }
}

void Lexer::init(const String &_flieName, const char *_sourceCode)
{
    fileName.shallowCopy(_flieName);
    sourceCode = _sourceCode;
    init();
}

void Lexer::init(const String &_flieName)
{
    fileName.shallowCopy(_flieName);
    readFile();
    init();
}

void Lexer::init()
{
    nextCharPtr = (char *)(sourceCode + 1);
    curChar = sourceCode[0];
    pos.line = 1;
    pos.column = 1;
    advance(); // 推进四次,到curToken
    advance();
    advance();
    advance();
}

void Lexer::setTokenPosition()
{
    PTOKEN.extract.setData(nextCharPtr - 1);
    PTOKEN.pos = pos;
}

void Lexer::advance()
{
    // 推进Token位置
    preToken = curToken;
    curToken = nextToken;
    nextToken = secondToken;
    secondToken = PTOKEN;
    // 跳过空格
    skipBlanks();
    // 设置token文本的起始位置
    setTokenPosition();
    // 初始化tokenType
    PTOKEN.type = TokenType::eof;
    // 循环判断字符
    while (curChar != '\0')
    {
        switch (curChar)
        {
        case '+':
            PTOKEN.type = TokenType::add;
            break;
        case '-':
            PTOKEN.type = TokenType::sub;
            break;
        case '*':
            PTOKEN.type = TokenType::mul;
            break;
        case '%':
            PTOKEN.type = TokenType::mod;
            break;
        case '^':
            PTOKEN.type = TokenType::power;
            break;
        case '.':
            PTOKEN.type = TokenType::dot;
            break;
        case ',':
            PTOKEN.type = TokenType::comma;
            break;
        case '(':
            PTOKEN.type = TokenType::lpare;
            break;
        case ')':
            PTOKEN.type = TokenType::rpare;
            break;
        case '[':
            PTOKEN.type = TokenType::lbracket;
            break;
        case ']':
            PTOKEN.type = TokenType::rbracket;
            break;
        case '{':
            PTOKEN.type = TokenType::lbrace;
            break;
        case '}':
            PTOKEN.type = TokenType::rbrace;
            break;
        case ';':
            PTOKEN.type = TokenType::end;
            break;
        case ':':
            PTOKEN.type = TokenType::colon;
            break;
        case '?':
            PTOKEN.type = TokenType::question;
            break;
        case '!':
            PTOKEN.type = matchNextChar('=') ? TokenType::notEqual : TokenType::logicNot;
            break;
        case '<':
            PTOKEN.type = matchNextChar('=') ? TokenType::lessEqual : TokenType::less;
            break;
        case '>':
            PTOKEN.type = matchNextChar('=') ? TokenType::greateEqual : TokenType::greate;
            break;
        case '=':
            PTOKEN.type = matchNextChar('=') ? TokenType::equal : TokenType::assign;
            break;
        case '/':
            if (matchNextChar('/'))
            {
                skipAlineComment();
                goto flag_handle;
            }
            else if (matchNextChar('*'))
            {
                skipBlockComment();
                goto flag_handle;
            }
            else
            {
                goto flag_normal;
            }
        flag_handle:
            skipBlanks();
            setTokenPosition();
            continue;
        flag_normal:
            PTOKEN.type = TokenType::div;
            break;
        case '"':
            // 字符串的情况
            PTOKEN.type = TokenType::str;
            parseString();
            return;
        default:
            //标识符或关键词或数字的情况
            if (isalpha(curChar) || curChar == '_')
            {
                parseId(TokenType::unknown);
            }
            else if (isdigit(curChar))
            {
                parseNum();
            }
            else
            {
                reportError(ErrorId::unsupportedChar, true, this, curChar);
            }
            return;
        }
        // 设置token的extract.data
        PTOKEN.extract.setLength((uint32_t)(nextCharPtr - PTOKEN.extract.getData()));
        // 取下一个字符
        getNextChar();
        return;
    }
}

void Lexer::getNextChar()
{
    if (curChar != '\0')
    {
        pos.column++;        // 推进列号
        if (curChar == '\n') // 遇到了换行符
        {
            pos.line++;     // 推进行号
            pos.column = 1; // 重置列号
        }
        curChar = *nextCharPtr++; // 推进字符
    }
}

void Lexer::skipBlanks()
{
    while (isspace(curChar))
    {
        getNextChar();
    }
}

void Lexer::skipAlineComment()
{
    while (curChar != '\0')
    {
        if (curChar == '\n')
        {
            getNextChar();
            return;
        }
        getNextChar();
    }
}

void Lexer::skipBlockComment()
{
    while (curChar != '\0')
    {
        if (curChar == '*' && matchNextChar('/'))
        {
            getNextChar();
            return;
        }
        getNextChar();
    }
}

bool Lexer::matchNextChar(char match)
{
    if (*nextCharPtr != match)
    {
        return false;
    }
    getNextChar();
    return true;
}

TokenType Lexer::idOrKeyword(String &str)
{
    TokenType type = isKeyword(str);
    type == TokenType::unknown ? type = TokenType::id : type; //用三元操作,一行完成,方便
    return type;
}

void Lexer::parseId(TokenType type)
{
    while (isalnum(curChar) || curChar == '_')
    {
        getNextChar();
    }
    uint32_t length = (uint32_t)(nextCharPtr - PTOKEN.extract.getData() - 1);
    PTOKEN.extract.setLength(length);
    PTOKEN.type = (type != TokenType::unknown ? type : idOrKeyword(PTOKEN.extract));
}

void Lexer::parseNum()
{
    uint8_t dotNum = 0;
    while (isdigit(curChar) || curChar == '.')
    {
        if (curChar == '.')
        {
            dotNum++;
        }
        getNextChar();
    }
    if (dotNum > 1)
    {
        reportError(ErrorId::invalidNumberFormat, true, this);
    }
    uint32_t length = (uint32_t)(nextCharPtr - PTOKEN.extract.getData() - 1);
    PTOKEN.type = TokenType::num;
    PTOKEN.extract.setLength(length);
    PTOKEN.value = Value(strtod(PTOKEN.extract.getData(), nullptr));
}

void Lexer::parseString()
{
    getNextChar();
    while (curChar != '\0' && curChar != '\n' && curChar != '"')
    {
        getNextChar();
    }
    if (curChar == '\0' || curChar == '\n')
    {
        reportError(ErrorId::unterminatedString, true, this);
    }
    getNextChar();
    uint32_t length = (uint32_t)(nextCharPtr - PTOKEN.extract.getData() - 1);
    PTOKEN.type = TokenType::str;
    PTOKEN.extract.setLength(length);
    PTOKEN.value = Value(String(PTOKEN.extract.getData() + 1, length - 2));
}

void Lexer::readFile()
{
    if (fileName.getData() == nullptr)
    {
        ErrorUnreachableBranch();
        return;
    }
    FILE *fp = fopen(fileName.getData(), "r");
    if (fp == nullptr)
    {
        reportError(ErrorId::failToReadFile, true, nullptr, fileName.getData());
        return;
    }
    fseek(fp, 0L, SEEK_END);
    uint32_t fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char *_sourceCode = (char *)calloc(fileSize + 1, 1); // 这里最好用calloc不然会出bug(new出来的未清零)
    fread(_sourceCode, 1, fileSize, fp);
    _sourceCode[fileSize] = '\0';
    fclose(fp);
    sourceCode = _sourceCode;
}
