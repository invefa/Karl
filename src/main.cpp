#include "value.hpp"
#include "lexer.hpp"
#include "vm.hpp"
#include "error.hpp"
#include "bytecode.hpp"
#include <stdio.h>

#define TEST_SCRIPT_FILE "E:\\Projects\\karl\\script"

// TODO:lexer还需改进,目前的lexer还没有精准输出错误位置的能力

int main()
{
    Lexer lexer(String(TEST_SCRIPT_FILE));
    // printf(lexer.sourceCode);
    Stack<Token> tokenStream = 16;
    while (lexer.curToken.type != TokenType::eof)
    {
        printf("[%d:%d] ", lexer.pos.line, lexer.pos.column);
        printf("[%d:%d]%s\n", lexer.curToken.pos.line, lexer.curToken.pos.column,
               TokenTypeName[(int)lexer.curToken.type]);
        tokenStream.push(lexer.curToken);
        lexer.advance();
    }
    printf("%d\n", tokenStream.getUsedsize());
    printf("%d\n", tokenStream.getCapacity());

    int32_t operand = 12;

    Stack<byte> data;
    data.push(operand & 0xff);
    data.push((operand >> 8) & 0xff);
    data.push((operand >> 16) & 0xff);
    data.push(operand >> 24);

    int32_t _operand = *((int32_t *)data.getData());

    byte *ip = data.getData();

    int32_t result = 0;
    result = *(ip + 3);
    result = (result << 8) | *(ip + 2);
    result = (result << 8) | *(ip + 1);
    result = (result << 8) | *(ip + 0);
    printf("%d:%d:%d\n", operand, _operand, result);

    Instream ins;
    ins.data = new Stack<byte>(16);
    ins.push(123, 4);
    printf("%d\n", ins.pop(ins.data->getData(), 4));
    String str = "getter.";
    printf("%s:%s\n", str.getData(), (str * 10).getData());
    printf("%d\n", sizeof(String));
    printf("%d\n", sizeof(ValueType));
    delete ins.data;
}
