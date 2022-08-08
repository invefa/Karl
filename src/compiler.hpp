#ifndef _KARL_COMPILER_
#define _KARL_COMPILER_
#include "lexer.hpp"
#include "basic.hpp"
#include "bytecode.hpp"
#include "vm.hpp"

class CorrectFnCall // 函数调用字节码操作数校正
{
public:
    byte *pos = nullptr; // 将存的位置,是call指令后面一位的地址
    String name;         // 被调用的函数名,必须深复制取得
};

// 每个module都有且仅有一个CompileUnit来编译它,结果存到VM的module表中
class CompileUnit
{
public:
    Lexer lexer;                          // 词法分析器
    VM *vm;                               // 编译单元所服务的虚拟机
    Module *module;                       // 被编译出的module
    Fn *fn;                               // 当前正在编译的函数
    Instream instream;                    // 当前编译的函数的字节流封装
    uint32_t scopeDeep = 0;               // 当前在编译位置的定义域深度
    Stack<String> varsName = 1;           // 当前函数的所有局部变量名,data来自sourceCode,不释放
    Stack<uint32_t> varsScopeDeep = 1;    // 局部变量的作用域
    Stack<CorrectFnCall> correctlist = 4; // 函数调用校正表
    // 构造函数
    CompileUnit(String &_flieName, const char *_sourceCode, VM *vm);
    CompileUnit(String &_flieName, VM *vm);
    CompileUnit();

    // 析构函数
    ~CompileUnit();

    // 初始化函数
    void init(String &_flieName, const char *_sourceCode, VM *vm);
    void init(String &_flieName, VM *vm);

    // 方法
    void enterScope();  // 进入一个新的定义域
    void escapeScope(); // 退出当前定义域

    void addVar(String &name); // 加入一个局部变量编译信息

    // 修正函数
    void correctFnCall();

    // 编译入口函数
    void compile();

    // 分支类
    bool statements();
    bool statement();
    bool code_block();

    // 定义类
    bool define_head();       // 定义头,方便判定使用
    bool function_define();   // 函数定义
    uint8_t arglist_define(); // 返回的是参数个数
    bool arg_define();        // 仅仅是个判断
    bool var_define();        // 变量定义

    // 表达式类
    bool expr();
    bool arith_expr();
    bool comp_expr();
    bool factor();
    bool power();
    bool term();
    bool call();
    bool atom();

    // 这个返回值特殊一些,返回的是参数个数
    uint8_t arg_list();

    // 语句类
    bool if_statement();
    bool for_statement();
    bool while_statement();
    bool switch_statement();
    bool empty_statement(); // 留守分号即为空语句

    // 标识类
    bool break_sign();
    bool continue_sign();
    bool return_sign();

private:
    // 推进
    Token &advance();
    // 初始化
    void initData(String &_fileName, VM *_vm); // 两种init的相同部分
    // 匹配TokenType
    bool matchTokenType(TokenType type);
    bool _matchNextTokenType(TokenType type);

    // 断言TokenType
    void assertTokenType(TokenType type);
    void _assertNextTokenType(TokenType type);
};

// 编译器,与vm相同,每个项目仅会有一个compiler,会被用来管理编译顺序和校正函数的调用等等
class Compiler
{
public:
    // 公共成员
    Stack<CompileUnit> culist = 1; // 编译单元表
    VM *vm;                        // 编译器所服务的虚拟机

    // 方法
    compile();
};

#endif
