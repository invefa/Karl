#include "compiler.hpp"

// 对于会被多次使用的类成员变量和语句就用宏来简化
#define CTT lexer.curToken.type
#define CT lexer.curToken

#define NTT lexer.nextToken.type
#define NT lexer.nextToken

#define STT lexer.secondToken.type
#define ST lexer.secondToken

#define TTT lexer.thirdToken.type
#define TT lexer.thirdToken

#define caseFalseReturnFalse(sign) \
    if (!(sign))                   \
    {                              \
        return false;              \
    }

#define caseFalseReturnTrue(sign) \
    if (!(sign))                  \
    {                             \
        return true;              \
    }

#define caseTrueReturnTrue(sign) \
    if (sign)                    \
    {                            \
        return true;             \
    }

#define caseFalseReportError(sign, id, ...)         \
    if (!(sign))                                    \
    {                                               \
        reportError(id, true, this, ##__VA_ARGS__); \
    }

#define pieceCaseOfMatch(tokentype, func, bytecode) \
    case tokentype:                                 \
        advance();                                  \
        ret = arith_expr();                         \
        ErrorExpectedExpr();                        \
        instream.push(bytecode);                    \
        break

#define limitRange(idx, max)                             \
    if ((idx) > (max))                                   \
    {                                                    \
        reportError(ErrorId::idxOutOfLimit, true, this); \
    }

#define ErrorExpectedExpr() caseFalseReportError(ret, ErrorId::expectedExpr)

CompileUnit::CompileUnit(String &_fileName, const char *_sourceCode, VM *_vm)
{
    init(_fileName, _sourceCode, _vm);
}

CompileUnit::CompileUnit(String &_fileName, VM *_vm)
{
    init(_fileName, _vm);
}

CompileUnit::CompileUnit()
{
}

CompileUnit::~CompileUnit()
{
}

void CompileUnit::init(String &_fileName, const char *_sourceCode, VM *_vm)
{
    lexer.init(_fileName, _sourceCode);
    initData(_fileName, _vm);
}

void CompileUnit::init(String &_fileName, VM *_vm)
{
    lexer.init(_fileName);
    initData(_fileName, _vm);
}

bool CompileUnit::_matchNextTokenType(TokenType type)
{
    if (NTT == type)
    {
        advance();
        advance();
        return true;
    }
    return false;
}

bool CompileUnit::matchTokenType(TokenType type)
{
    if (CTT == type)
    {
        advance();
        return true;
    }
    return false;
}

void CompileUnit::assertTokenType(TokenType type)
{
    if (CTT != type)
    {
        reportError(ErrorId::assertTokenTypeFailure, true, this,
                    TokenTypeName[(uint32_t)type],
                    TokenTypeName[(uint32_t)CTT]);
    }
    advance(); // 断言成功则advance
}

void CompileUnit::_assertNextTokenType(TokenType type)
{
    if (NTT != type)
    {
        reportError(ErrorId::assertTokenTypeFailure, true, this,
                    TokenTypeName[(uint32_t)type],
                    TokenTypeName[(uint32_t)NTT]);
    }
    advance();
    advance(); // 断言成功则advance,不过这个是断言nextToken,所以advance两次
}

Token &CompileUnit::advance()
{
    lexer.advance();
    return lexer.preToken;
}

void CompileUnit::initData(String &_fileName, VM *_vm)
{
    vm = _vm;                          // loadin vm
    module = vm->modules.firstEmpty(); // loadin module
    fn = module->fnlist.getData();     // loadin fn
    instream.data = fn->data.instream; // loadin insteram
    module->name.deepCopy(_fileName);  // loadin moduleName
}

void CompileUnit::escapeScope()
{
    while (*varsScopeDeep.lastUsed() == scopeDeep)
    { // 把所有当前scope的变量pop出来
        varsName.pop();
        varsScopeDeep.pop();
    }
    if (scopeDeep == 0)
    {
        ErrorUnreachableBranch(); // 这种情况一般不可能发生
    }
    scopeDeep--; // 把scope降一位
}

void CompileUnit::enterScope()
{
    scopeDeep++;
}

void CompileUnit::addVar(String &name)
{
    if (varsName.search(name) != varsName.getUsedsize()) // 如果变量名已存在
    {
        reportError(ErrorId::varAlreadyDefined, true, this, name.deepCopy().getData());
    }
    String *tmp = varsName.firstEmpty();
    tmp->shallowCopy(name);
    varsScopeDeep.push(scopeDeep);
}

void CompileUnit::compile()
{
    while (CTT != TokenType::eof)
    {
        caseFalseReportError(define_head(), ErrorId::unknownStatement);
    }
}

bool CompileUnit::statements()
{
    while (CTT != TokenType::rbrace)
    {
        if (CTT == TokenType::eof) // statements一定是裹在大括号里,结束表示语法错误
        {
            reportError(ErrorId::assertTokenTypeFailure, true, this,
                        TokenTypeName[(uint32_t)TokenType::rbrace],
                        TokenTypeName[(uint32_t)TokenType::eof]);
        }
        caseFalseReturnTrue(statement());
        // 不匹配语句了就返回,此时必定匹配了至少一个语句,则返回true
    }
    return false; // 能到这里表明没有匹配任何语句,直接到了rbrace,则返回false
}

bool CompileUnit::statement()
{
    // 惰性匹配
    caseTrueReturnTrue(if_statement());
    caseTrueReturnTrue(for_statement());
    caseTrueReturnTrue(while_statement());
    caseTrueReturnTrue(empty_statement());
    caseTrueReturnTrue(code_block());

    // 特殊些的惰性匹配
    if (var_define())
    {
        assertTokenType(TokenType::end); // 变量定义后必定要分号结束
        return true;
    }
    else if (expr())
    {
        assertTokenType(TokenType::end); // 语句里的表达式后必定要分号结束
        return true;
    }
    else
    {
        if (CTT == TokenType::rbrace)
        { // code_block -> statements -> statement 于是只有右大括号才是结束标识,没匹配到而遇到rbrace则返回false
            return false;
        }
        // 不是上面一个if那种特殊情况的话就是未知语句喽
        reportError(ErrorId::unknownStatement, true, this);
    }
    ErrorUnreachableBranch(); // 无法到达的分支
    return false;
}

bool CompileUnit::code_block()
{
    if (CTT != TokenType::lbrace)
    {
        return false;
    }
    assertTokenType(TokenType::lbrace);
    statements(); // 大括号内可以无语句,于是不用管返回值
    assertTokenType(TokenType::rbrace);
    return true;
}

bool CompileUnit::define_head()
{
    caseFalseReturnFalse(isTypeKeyword(CTT) || CTT == TokenType::id);
    caseFalseReturnFalse(NTT == TokenType::id);
    if (scopeDeep > 0) // scopeDeep不为0,则当前处于函数内,即逻辑层,则不可能有函数定义,只能定义变量
    {
        return var_define();
    }
    if (STT == TokenType::lpare)
    {
        return function_define();
    }
    caseTrueReturnTrue(var_define());
    return false;
}

bool CompileUnit::function_define()
{
    if (scopeDeep > 0) // scopeDeep不为0,则当前处于函数内,即逻辑层,则不可能有函数定义,只能定义变量
    {
        return false;
    }
    caseFalseReturnFalse(isTypeKeyword(CTT) || CTT == TokenType::id);
    caseFalseReturnFalse(NTT == TokenType::id && STT == TokenType::lpare);
    fn = module->fnlist.firstEmpty();
    Signature *sign = &fn->sign;
    sign->ret = getCorTokenType(advance().type);
    return false;
}

uint8_t CompileUnit::arglist_define()
{
    return false;
}

bool CompileUnit::arg_define()
{
    caseFalseReturnFalse(isTypeKeyword(CTT) || CTT == TokenType::id);
    caseFalseReturnFalse(NTT == TokenType::id);
    return true;
}

bool CompileUnit::var_define()
{
    return false;
}

bool CompileUnit::expr()
{
    bool ret = comp_expr();
    caseFalseReturnFalse(ret);
    switch (CTT)
    {
        pieceCaseOfMatch(TokenType::logicAnd, comp_expr(), Bytecode::LAND);
        pieceCaseOfMatch(TokenType::logicOr, comp_expr(), Bytecode::LOR);
    }
    return true;
}

bool CompileUnit::comp_expr()
{
    bool ret = false;
    if (matchTokenType(TokenType::logicNot))
    {
        // 当前token为逻辑取反时,则强制匹配右表达式
        ret = comp_expr();
        ErrorExpectedExpr(); // 强制匹配
        instream.push(Bytecode::LNOT);
        return true;
    }
    ret = arith_expr();
    caseFalseReturnFalse(ret); // 惰性匹配
    while (true)
    {
        switch (CTT) // 下面是多个简单重复代码段
        {
            pieceCaseOfMatch(TokenType::equal, arith_expr(), Bytecode::EQUAL);
            pieceCaseOfMatch(TokenType::notEqual, arith_expr(), Bytecode::NEQUAL);
            pieceCaseOfMatch(TokenType::greate, arith_expr(), Bytecode::GREAT);
            pieceCaseOfMatch(TokenType::greateEqual, arith_expr(), Bytecode::GEQUAL);
            pieceCaseOfMatch(TokenType::less, arith_expr(), Bytecode::LESS);
            pieceCaseOfMatch(TokenType::lessEqual, arith_expr(), Bytecode::LEQUAL);
        default:
            goto flag_comp_expr_outside; // 这里写break只对switch有效,想跳出while(true)只能用goto
        }
    }
flag_comp_expr_outside:
    // 因为这里是惰性匹配与强制匹配的终点,惰性匹配遇到无法识别直接返回false,强制匹配遇到无法识别直接报错终止,所以这里设为true方可
    return true;
}

bool CompileUnit::arith_expr()
{
    bool ret = term();
    caseFalseReturnFalse(ret); // 惰性匹配
    while (true)
    {
        switch (CTT) // 下面是多个简单重复代码段
        {
            pieceCaseOfMatch(TokenType::add, term(), Bytecode::ADD);
            pieceCaseOfMatch(TokenType::sub, term(), Bytecode::SUB);
        default:
            goto flag_arith_expr_outside; // 这里写break只对switch有效,想跳出while(true)只能用goto
        }
    }
flag_arith_expr_outside:
    // 因为这里是惰性匹配与强制匹配的终点,惰性匹配遇到无法识别直接返回false,强制匹配遇到无法识别直接报错终止,所以这里设为true方可
    return true;
}

bool CompileUnit::term()
{
    bool ret = factor();
    caseFalseReturnFalse(ret); // 惰性匹配
    while (true)
    {
        switch (CTT) // 下面是多个简单重复代码段
        {
            pieceCaseOfMatch(TokenType::mul, factor(), Bytecode::MUL);
            pieceCaseOfMatch(TokenType::div, factor(), Bytecode::DIV);
            pieceCaseOfMatch(TokenType::mod, factor(), Bytecode::MOD);
        default:
            goto flag_term_outside; // 这里写break只对switch有效,想跳出while(true)只能用goto
        }
    }
flag_term_outside:
    // 因为这里是惰性匹配与强制匹配的终点,惰性匹配遇到无法识别直接返回false,强制匹配遇到无法识别直接报错终止,所以这里设为true方可
    return true;
}

bool CompileUnit::factor()
{
    bool ret = false;
    switch (CTT) // 下面是多个简单重复代码段
    {
    case TokenType::add: // 无操作,不压入指令,所以不能用pieceCaseOfMatch
        advance();
        ret = factor();
        ErrorExpectedExpr(); // 强制匹配
        break;
        pieceCaseOfMatch(TokenType::sub, factor(), Bytecode::TON);
    default:
        ret = power(); // 不是取相反数和无运算就是这个,这个也不是就是非匹配段,则return false
        caseFalseReturnFalse(ret);
    }
    return true; // 通过了两种情况任意一个就到达了这里
}

bool CompileUnit::power()
{
    bool ret = call();
    caseFalseReturnFalse(ret); // 惰性匹配
    while (true)
    {
        switch (CTT) // 下面是多个简单重复代码段
        {
            pieceCaseOfMatch(TokenType::dot, factor(), Bytecode::UNK);
            // TODO 类成员的调用字节码
            pieceCaseOfMatch(TokenType::power, factor(), Bytecode::POW);
        default:
            goto flag_power_outside; // 这里写break只对switch有效,想跳出while(true)只能用goto
        }
    }
flag_power_outside:
    // 因为这里是惰性匹配与强制匹配的终点,惰性匹配遇到无法识别直接返回false,强制匹配遇到无法识别直接报错终止,所以这里设为true方可
    return true;
}

bool CompileUnit::call()
{
    bool ret = atom();
    caseFalseReturnFalse(ret); // 惰性匹配
    uint8_t _argCount;
    if (CTT == TokenType::lpare)
    {
        advance();
        _argCount = arg_list(); // 非强制匹配,可有可无
        assertTokenType(TokenType::rpare);
    }
    return true;
}

bool CompileUnit::atom()
{
    switch (CTT) // 下面是一些终止符
    {
    case TokenType::str: // 字符串字面量
    case TokenType::num: // 数值字面量
        instream.push(Bytecode::LOAD_CONST,
                      module->constants.registerData(CT.value), 4);
        // 这里就不需要范围限制了,常量读取的operand是4字节的
        advance();
        break;
    case TokenType::id: // 变量值引用
    {
        uint32_t idx = varsName.search(CT.extract);
        if (idx == varsName.getUsedsize()) // 寻找变量,如果不存在此变量则报错
        {
            reportError(ErrorId::varNotDefined, true, this,
                        CT.extract.deepCopy().getData());
            // 这里需要深复制一下,不然会从该token的开始位置打印到代码结束
            return false;
        } // TODO 支持全局变量

        limitRange(idx, UINT16_MAX); // 限制一下范围
        instream.push(Bytecode::LOAD_VAR, idx, 2);
        advance();
        break;
    }
    default:
        return false;
    }
    return true;
}

uint8_t CompileUnit::arg_list()
{
    ErrorUnreachableBranch();
    return false;
}

bool CompileUnit::if_statement()
{
    ErrorUnreachableBranch();
    return false;
}

bool CompileUnit::for_statement()
{
    ErrorUnreachableBranch();
    return false;
}

bool CompileUnit::while_statement()
{
    ErrorUnreachableBranch();
    return false;
}

bool CompileUnit::switch_statement()
{
    ErrorUnreachableBranch();
    return false;
}

bool CompileUnit::empty_statement()
{
    if (CTT != TokenType::end)
    {
        return false;
    }
    advance();
    return true;
}
