# ccompiler 编译器

> 项目简介：ccompiler 是一个将 C99 源码编译为 RISC-V 32 位汇编的编译器，采用经典三段式架构（前端 → 中端 → 后端）。  
> 开发语言：C++17 | 构建系统：CMake | 输出可执行文件：cc

---

## 目录

1. [项目整体架构](#1-项目整体架构)
2. [Support 基础设施](#2-support-基础设施)
   - 2.1 [Arena 内存分配器](#21-arena-内存分配器)
   - 2.2 [CCError 异常处理](#22-ccerror-异常处理)
   - 2.3 [DiagnosticEngine 诊断引擎](#23-diagnosticengine-诊断引擎)
3. [Frontend 前端](#3-frontend-前端)
   - 3.1 [AST 系统](#31-ast-系统)
   - 3.2 [类型系统](#32-类型系统)
   - 3.3 [声明系统](#33-声明系统)
   - 3.4 [语句系统](#34-语句系统)
   - 3.5 [表达式系统](#35-表达式系统)
   - 3.6 [词法分析器](#36-词法分析器)
   - 3.7 [语法分析器](#37-语法分析器)
   - 3.8 [语义分析](#38-语义分析)
4. [MiddleEnd 中端 (SSA IR)](#4-middleend-中端-ssa-ir)
5. [Backend 后端 (RISC-V 代码生成)](#5-backend-后端-risc-v-代码生成)
6. [编译管线总览](#6-编译管线总览)

---

## 1. 项目整体架构

```
                     C 源代码
                         │
                         ▼
    ┌─────────────────────────────────┐
    │          Frontend 前端           │
    │  Scanner → Parser → Sema → AST  │
    │  词法分析   语法分析   语义分析    │
    └─────────────────────────────────┘
                         │
                         ▼
    ┌─────────────────────────────────┐
    │         MiddleEnd 中端          │
    │   IRBuilder → DominatorTree     │
    │   AST→SSA IR    支配树分析       │
    │              → Mem2Reg 优化      │
    └─────────────────────────────────┘
                         │
                         ▼
    ┌─────────────────────────────────┐
    │          Backend 后端             │
    │   InstSelect → RegAlloc → Emit  │
    │   指令选择    寄存器分配   汇编输出 │
    └─────────────────────────────────┘
                         │
                         ▼
                   RISC-V 汇编
```

### 1.1 模块依赖关系

```
main.cpp
   └── frontend (libfrontend)
   │      └── support (libsupport)
   └── middleend (libmiddleend)
   │      └── frontend
   │           └── support
   └── backend (libbackend)
          └── middleend
               └── frontend
                    └── support
```

### 1.2 命名空间

全部代码位于 `namespace ccompiler { ... }` 中，后端目标相关的 RISC-V 寄存器枚举为嵌套命名空间 `RiscvReg`。

---

## 2. Support 基础设施

**目录**：`src/support/memory/`、`src/support/ccerror/`  
**核心文件**：`arena.h/cpp`、`ccerror.h/cpp`

### 2.1 Arena 内存分配器

`Arena` 是**单例 + 线程局部存储**的线性内存分配器（Bump Allocator），是编译器性能的关键。

#### 2.1.1 类接口

```cpp
class Arena {
public:
    static Arena* Instance();                       // Meyer's 单例
    ~Arena();                                        // 调用 release()

    template<typename T, typename... Args>
    static T* make(Args&&... args);                  // 在 TLS Arena 上构造对象

    template<typename T>
    static void destroy(T* obj);                     // 仅调用析构函数

    void* allocate(std::size_t bytes,                // 核心分配
                   std::size_t alignment);
    void release();                                  // 释放所有 chunk
private:
    std::size_t m_chunkSize;                         // 默认 60KB
    std::vector<char*> m_chunks;                     // 所有已分配 chunk
    char* m_current;                                 // 当前可用位置
    char* m_end;                                     // 当前 chunk 末尾
    Arena(std::size_t chunkSize = 60 * 1024);
    void allocateNewChunk(std::size_t minSize = 0);
};
```

#### 2.1.2 分配算法：线性分配（Bump Allocation）

```
allocate(bytes, alignment):
  1. alignedPtr = align_up(m_current, alignment)
     alignedPtr = (m_current + alignment - 1) & ~(alignment - 1)
  2. needed = (alignedPtr - m_current) + bytes
  3. if needed > (m_end - m_current):
       allocateNewChunk(max(bytes, m_chunkSize))
       goto 1
  4. m_current = alignedPtr + bytes
  5. return alignedPtr
```

**算法特点**：
- **零碎片**：整块分配、整块释放，无内存碎片。
- **O(1) 分配**：仅常数次整数运算，无空闲链表遍历。
- **批量释放**：`release()` 遍历所有 chunk 执行 `delete[]`，O(N)。
- **无单个 free**：编译器场景下 AST/IR 节点生命周期一致，无需逐个释放。

#### 2.1.3 `make<T>` 模板工厂

`Arena::make<T>(args...)` 结合了四项关键技术：

| 技术 | 作用 |
|------|------|
| **完美转发** | `Args&&` + `std::forward` 保持值类别 |
| **Placement new** | `new(mem) T(...)` 在已分配内存上构造 |
| **线程局部存储** | `static thread_local Arena`，每线程独立实例 |
| **对齐保证** | `alignof(T)` 传入 allocate |

```cpp
// 典型调用（项目中使用超过 100 处）
auto node = Arena::make<IntegerLiteral>(ty, 42);
auto inst = Arena::make<LoadInst>(ty, addr, block);
auto sym  = Arena::make<Symbol>(name, scope);
```

### 2.2 CCError 异常处理

```cpp
class CCError : public std::exception {
    const char* what() const noexcept override;
    CCError(const std::string& val);
    // 完整拷贝/移动语义
private:
    std::string err_;
};
```

- 继承自 `std::exception`，通过 `std::string` 存储错误描述。
- 用于词法分析器报告错误，例如 `throw CCError("invalid character")`。

### 2.3 DiagnosticEngine 诊断引擎

**目录**：`src/support/diag/`  
**核心文件**：`diag.h/cpp`

DiagnosticEngine 是**集中式编译错误/警告收集和输出系统**，替代了之前分散在各模块的 stderr 直接输出模式。它位于 Support 层，可以被前端、中端、后端所有模块共享使用。

#### 2.3.1 类接口

```cpp
class DiagnosticEngine {
public:
    enum Level { Note, Warning, Error };

    // 核心报告接口
    void report(Level, const DiagLocation& loc, const std::string& msg);
    void report(Level, const std::string& msg);

    // 便捷方法
    void error(...), warning(...), note(...);

    // 查询
    int getErrorCount() const;
    int getWarningCount() const;
    bool hasErrors() const;

    void printAll(std::ostream& os = std::cerr);  // 输出所有诊断
    void clear();                                  // 重置
};
```

#### 2.3.2 DiagLocation — 诊断位置

```cpp
struct DiagLocation {
    std::string filename;  // 源文件名
    int line;              // 行号
    int column;            // 列号
};
```

与前端 `SourceLocation` 结构相同，但定义在 Support 层以避免跨层依赖。

#### 2.3.3 格式化输出

```
filename:line:col: error: message
filename:line:col: warning: message
filename:line:col: note: message
```

使用 ANSI 彩色输出（红色 Error、黄色 Warning、青色 Note），与现有 `sytaxError`/`semaError` 风格一致。

#### 2.3.4 编译管线集成

```
Parser::run():
  1. 词法分析 + 语法分析
  2. SemaAnalyzer::run()       ← 记录诊断到 DiagnosticEngine
  3. if hasErrors(): throw      ← 终止编译

main.cpp compile():
  try:
    parser.run()
    if diag->hasErrors(): return false
    // 继续 IR 生成和后端...
  catch(CCError& e):
    输出错误并退出
```

---

## 3. Frontend 前端

**目录**：`src/frontend/ast/`、`lexer/`、`parser/`、`semantic/`  
**核心文件**：`ast.h/cpp`、`type.h/cpp`、`decl.h`、`stmt.h`、`expr.h`、`token.h/cpp`、`scanner.h/cpp`、`source.h/cpp`、`parse.h/cpp`、`symbol.h/cpp`

### 3.1 AST 系统

#### 3.1.1 AstNode — 所有节点的基类

```cpp
class AstNode {
    enum NodeKind { /* 42 种节点类型 */ };
    AstNode(NodeKind nk);
    virtual NodeKind getKind() const;
    virtual std::any accept(ASTVisitor*) = 0;  // 访问者模式
};
```

**节点分类**：

| 类别 | 节点数 | 节点类型 |
|------|--------|----------|
| 翻译单元 | 1 | TranslationUnitDecl |
| 声明 | 13 | NamedDecl, LabelDecl, ValueDecl, DeclaratorDecl, VarDecl, ParmVarDecl, FunctionDecl, TypedefDecl, FieldDecl, EnumConstantDecl, TagDecl, EnumDecl, RecordDecl |
| 表达式 | 14 | DeclRefExpr, IntegerLiteral, CharacterLiteral, FloatingLiteral, StringLiteral, ParenExpr, ArraySubscriptExpr, CallExpr, MemberExpr, CompoundLiteralExpr, UnaryOperator, CastExpr, BinaryOperator, ConditionalOperator |
| 语句 | 15 | LabelStmt, CaseStmt, DefaultStmt, CompoundStmt, DeclStmt, ExprStmt, IfStmt, SwitchStmt, WhileStmt, DoStmt, ForStmt, GotoStmt, ContinueStmt, BreakStmt, ReturnStmt |

#### 3.1.2 ASTVisitor — 访问者模式

```cpp
class ASTVisitor {
    virtual std::any visit(TranslationUnitDecl*) = 0;
    virtual std::any visit(VarDecl*) = 0;
    virtual std::any visit(FunctionDecl*) = 0;
    virtual std::any visit(CompoundStmt*) = 0;
    virtual std::any visit(IfStmt*) = 0;
    virtual std::any visit(WhileStmt*) = 0;
    virtual std::any visit(BinaryOpExpr*) = 0;
    virtual std::any visit(IntegerLiteral*) = 0;
    // ... 共 41 个纯虚函数
};
```

**双重分派机制**：每个节点类的 `accept` 通过宏 `DEFINE_ACCEPT_METHOD` 统一实现，调用 `vt->visit(this)`，由编译器的重载决议选择正确的 visit 版本。

```cpp
#define DEFINE_ACCEPT_METHOD(CLASS, VISITOR, RET) \
RET CLASS::accept(VISITOR* vt) { \
    if (vt) { return vt->visit(this); } \
    return RET(); \
}
```

#### 3.1.3 AstCtx — AST 上下文

```cpp
struct AstCtx {
    TranslationUnitDecl* ir;       // AST 根节点
    SymbolTableContext* symbol;    // 符号表上下文
};
```

这是解析完成后传递给 IRBuilder 的数据结构。

### 3.2 类型系统

#### 3.2.1 QualType

`QualType` 是类型系统的核心包装类，采用**值语义 + 类型限定符分离存储**：

```cpp
class QualType {
    uint8_t m_qualSpec;     // const/restrict/volatile 位掩码
    Type* m_type;           // 裸指针（生命周期由 Arena 管理）
public:
    operator bool() const;
    Type& operator*() const;
    Type* operator->() const;
    template<typename T> T* as() const;   // dynamic_cast 向下转型
    bool operator==(const QualType&) const;  // 指针地址比较
};
```

**关键设计**：
- 限定符与底层类型分离：`const int` 和 `int` 共享同一个 `IntegerType` 对象。
- `as<T>()` 类似 LLVM 的 `dyn_cast`：`type.as<FunctionType>()`。
- 类型等价判断 `operator==` 采用**引用等价**（指针地址比较）。

#### 3.2.2 类型层次

```
Type (抽象基类)
├── VoidType           — void 类型
├── IntegerType        — char, short, int, long, long long, _Bool
├── FloatType          — float, double, long double
├── FunctionType       — 函数类型 (含返回类型和参数类型列表)
├── PointerType        — 指针类型
├── ArrayType          — 数组类型
└── RecordType         — struct/union 类型
```

**TypeKind 枚举**：`TK_Void, TK_Int, TK_Float, TK_Function, TK_Pointer, TK_Array, TK_Record`

**核心方法**：
- `isSame(const Type*)`：类型等价性判断（递归比较结构）。
- `isArithmeticType()`：是否为算术类型。
- `isIntegerType()`：是否为整型。
- `getSize()`：类型大小（字节）。
- `getAlignment()`：类型对齐要求。

#### 3.2.3 声明说明符枚举

```cpp
enum StorageClass { N_SCLASS, TYPEDEF=2, EXTERN=4, STATIC=8, AUTO=16, REGISTER=32 };
enum TypeSpecifier { VOID, CHAR, SHORT, INT, LONG, LONGLONG, FLOAT, DOUBLE, SIGNED, UNSIGNED, _BOOL, _COMPLEX, STRUCTORUNION, ENUM, TYPEDEFNAME };
enum TypeQualifier { CONST, RESTRICT, VOLATILE };
enum FuncSpecifier { INLINE };
```

所有枚举使用位掩码设计，支持组合。

### 3.3 声明系统

```
Decl (AstNode)
└── TranslationUnitDecl  — 翻译单元（源文件）
└── NamedDecl            — 具名声明基类
    ├── LabelDecl        — 标签声明（goto 目标）
    └── ValueDecl        — 含值声明基类
        ├── EnumConstantDecl — 枚举常量
        ├── FieldDecl        — struct/union 字段
        └── DeclaratorDecl   — 含声明符的声明基类
            ├── VarDecl      — 变量声明（含初始化表达式）
            └── FunctionDecl — 函数声明（含参数和函数体）
└── TypedefDecl           — typedef 声明
    ├── EnumDecl          — 枚举定义
    └── RecordDecl        — struct/union 定义
```

### 3.4 语句系统

| 语句类 | 关键成员 | 说明 |
|--------|----------|------|
| CompoundStmt | `std::vector<Stmt*> m_bodys` | 复合语句（花括号内的语句列表） |
| DeclStmt | `Decl* m_decl` | 声明语句 |
| ExprStmt | `Expr* m_expr` | 表达式语句 |
| IfStmt | `Expr* cond, Stmt* then, Stmt* else` | if-else 条件分支 |
| WhileStmt | `Expr* cond, Stmt* body` | while 循环 |
| DoStmt | `Expr* cond, Stmt* body` | do-while 循环 |
| ForStmt | `Stmt* init, Expr* cond, Expr* update, Stmt* body` | for 循环 |
| ReturnStmt | `Expr* retExpr` | return 语句 |
| BreakStmt | — | break 语句 |
| ContinueStmt | — | continue 语句 |
| GotoStmt | `char* label` | goto 语句 |
| LabelStmt | `char* key, Stmt* val` | 标签语句 |
| CaseStmt | `Expr* cond, Stmt* val` | case 分支 |
| DefaultStmt | `Stmt* val` | default 分支 |

### 3.5 表达式系统

```
Expr (继承 Stmt)
├── IntegerLiteral           — 整数字面量
├── CharacterLiteral         — 字符字面量
├── FloatingLiteral          — 浮点字面量
├── StringLiteral            — 字符串字面量
├── DeclRefExpr              — 变量引用（含 NamedDecl* 指针）
├── ParenExpr                — 括号表达式
├── BinaryOpExpr             — 二元表达式（42 种操作符）
├── UnaryOpExpr              — 一元表达式（11 种操作符）
├── ConditionalExpr          — 条件表达式 (?:)
├── CompoundLiteralExpr      — 复合字面量
├── CastExpr                 — 类型转换
├── ArraySubscriptExpr       — 数组下标
├── CallExpr                 — 函数调用
└── MemberExpr               — 结构体成员访问
```

**BinaryOpExpr 操作符枚举**（42 种）：

```
Assign_, Addition_, Subtraction_, Multiplication_, Division_, Modulus_,
LShift_, RShift_, Less_, Greater_, Less_Equal_, Greater_Equal_,
Equality_, Inequality_, BitWise_AND_, BitWise_XOR_, BitWise_OR_,
Logical_AND_, Logical_OR_, Comma,
Mult_Assign_, Div_Assign_, Mod_Assign_, Add_Assign_, Sub_Assign_,
LShift_Assign_, RShift_Assign_, BitWise_AND_Assign_, BitWise_XOR_Assign_,
BitWise_OR_Assign_
```

**UnaryOpExpr 操作符枚举**（11 种）：

```
Addition_, Subtraction_, Logical_NOT_, BitWise_NOT_,
Pre_Increment_, Pre_Decrement_, Post_Increment_, Post_Decrement_,
Multiplication_ (解引用), BitWise_AND_ (取地址), SIZEOF_
```

### 3.6 词法分析器（Lexer）

**目录**：`src/frontend/lexer/`  
**文件**：`token.h/cpp`, `scanner.h/cpp`, `source.h/cpp`

#### 3.6.1 Token 系统

**TokenKind 枚举**使用 **X-macro 模式**（`MACROS_TABLE`）：

```cpp
#define MACROS_TABLE \
    X_MACROS(Alignof, "alignof") \
    X_MACROS(Auto, "auto") \
    X_MACROS(Break, "break") \
    // ... 共 64 个 token 类型
    X_MACROS(Identifier, "Identifier") \
    X_MACROS(IntegerLiteral, "IntegerLiteral") \
    X_MACROS(FloatingLiteral, "FloatingLiteral") \
    X_MACROS(StringLiteral, "StringLiteral")

enum class TokenKind {
    #define X_MACROS(a, b) a,
    MACROS_TABLE
    #undef X_MACROS
};
```

**Token 类**：
```cpp
class Token {
    TokenKind m_kind;
    std::string m_content;           // 原始字符串内容
    SourceLocation m_location;       // 源码位置
    static const std::unordered_map<std::string, TokenKind> KeyWordMap;
};
```

- `KeyWordMap` 将关键字字符串映射到对应的 `TokenKind`，用于 scanner 的标识符识别。

**TokenSequence**：
```cpp
class TokenSequence {
    Token* getToken(unsigned i);
    void addToken(Token* tok);
    unsigned getSize();
};
```

#### 3.6.2 Source 源码字符流

```cpp
class SourceLocation {
    unsigned m_lineNo;    // 行号（从 1 开始）
    unsigned m_colNo;     // 列号（从 1 开始）
};

class Source {
    std::vector<char> m_sourceCode;  // 源码字符缓冲区
    unsigned m_offset;               // 当前读取偏移
    SourceLocation m_location;       // 当前位置

    char peekChar();                 // 查看当前字符不移位
    char getChar();                  // 获取当前字符并前进
    void ungetChar();                // 回退一个字符
    void skipWhitespace();           // 跳过空白
    bool eof();
};
```

**SourceLocation 的追踪**：每次 `getChar()` 时递增列号，遇到 `\n` 时行号 +1 且列号清零。

#### 3.6.3 Scanner 扫描器

```cpp
class scanner {
    Source* m_source;
    TokenSequence m_tokenSeq;
    bool scan();                     // 主扫描入口
    Token* readIdentifier();         // 读取标识符/关键字
    Token* readInteger();            // 读取整数字面量
    Token* readString();             // 读取字符串字面量
    Token* readCharConstant();       // 读取字符常量
    Token* readComment();            // 读取注释
};
```

**扫描算法**：一次性扫描所有 token，存入 `m_tokenSeq`。

```
scan():
  循环直到 EOF:
    c = peekChar()
    switch c:
      'a'-'z', 'A'-'Z', '_' → readIdentifier()
      '0'-'9'                 → readInteger()
      '\"'                    → readString()
      '\''                    → readCharConstant()
      '/', '*'                → readComment()
      '+', '-', ...           → readPunctuator()
      其他                    → error / skip
```

**关键字识别算法**：
1. `readIdentifier()` 读取完整的标识符字符串。
2. 在 `Token::KeyWordMap` 中查找。若命中，设为对应 `TokenKind`；否则为 `Identifier`。

**数字字面量扫描**：
- 支持十进制、十六进制（`0x`/`0X`）、八进制（`0` 开头）。
- 支持整数后缀（`u`/`U`/`l`/`L`/`ll`/`LL`），由 `readInteger()` 处理。
- 转换为 `uint64_t` 存储。

### 3.7 语法分析器（Parser）

**目录**：`src/frontend/parser/`  
**文件**：`parse.h/cpp`（约 1824 行）

#### 3.7.1 Parser 类接口

```cpp
class Parser {
    TokenSequence* m_tokenSeq;
    unsigned m_tokenPos;             // 当前 token 位置
    ScopeManager* m_scopeManager;    // 作用域管理器
    SemaAnalyzer* m_semAnalyzer;     // 语义分析器

    // 入口
    bool run(const std::string& filename);
    AstCtx getAstCtx();              // 获取 AST + 符号表

    // 核心解析函数（递归下降）
    TranslationUnitDecl* parseTranslationUnit();
    Decl* parseDeclaration();
    Decl* parseFunctionDefinition(TypeSpecifier, DeclaratorDecl*);
    Decl* parseVarDeclaration(TypeSpecifier, DeclaratorDecl*, bool isFuncParam = false);
    Stmt* parseStatement();
    Stmt* parseCompoundStatement();
    Stmt* parseIfStatement();
    Stmt* parseWhileStatement();
    Stmt* parseForStatement();
    Stmt* parseReturnStatement();
    Expr* parseExpression();
    Expr* parseAssignmentExpression();
    Expr* parseConditionalExpression();
    Expr* parseLogicalOrExpression();
    // ... 按优先级逐级下降
    Expr* parsePrimaryExpression();

    // 辅助方法
    Token* getCurToken();            // 查看当前 token
    Token* getNextToken();           // 前进一位并返回
    bool match(TokenKind);           // 匹配并前进
    void advance();                  // 前进不检查
    void error(const std::string&);  // 错误报告
};
```

#### 3.7.2 ScopeManager 作用域管理

使用 RAII 模式管理作用域的创建与销毁：

```cpp
class ScopeManager {
    Parser* parent_;
public:
    ScopeManager(Parser*, Scope::ScopeType st);   // 创建新作用域
    ~ScopeManager();                                // 退出作用域
};
```

例如在解析复合语句时：
```cpp
{  // ScopeManager 构造 → 创建新作用域
    int x;
    {  // ScopeManager 构造 → 嵌套作用域
        int y;
    }  // ScopeManager 析构 → 退出嵌套作用域
}  // ScopeManager 析构 → 退出外层作用域
```

#### 3.7.3 解析算法：递归下降

Parser 采用**递归下降解析**，严格遵循 C99 标准的运算符优先级层次，为每一层优先级实现一个解析函数：

```
parseExpression()
  └── parseAssignmentExpression()
       └── parseConditionalExpression()
            └── parseLogicalOrExpression()
                 └── parseLogicalAndExpression()
                      └── parseBitwiseOrExpression()
                           └── parseBitwiseXorExpression()
                                └── parseBitwiseAndExpression()
                                     └── parseEqualityExpression()
                                          └── parseRelationalExpression()
                                               └── parseShiftExpression()
                                                    └── parseAdditiveExpression()
                                                         └── parseMultiplicativeExpression()
                                                              └── parseUnaryExpression()
                                                                   └── parsePostfixExpression()
                                                                        └── parsePrimaryExpression()
```

**关键解析函数详解**：

**`parseTranslationUnit()`**：
```
while (有 token):
  if TokenKind == FunctionSpec/TypeSpec/StorageClass:
    parseDeclaration()
  else:
    error()
```

**`parseDeclaration()`**：
```
1. 解析类型说明符（TypeSpecifier）
2. 解析声明符（DeclaratorDecl）：标识符 + 指针/数组/函数修饰
3. 根据上下文判断：
   - 若后跟 '(' → parseFunctionDefinition()    // 函数定义
   - 若后跟 ';' 或 '=' → parseVarDeclaration()  // 变量声明
   - 若后跟 '{' → parseFunctionDefinition()     // 函数定义
```

**`parseFunctionDefinition()`**：解析返回类型、函数名、参数列表、函数体 CompoundStmt，创建 `FunctionDecl`。

**`parseIfStatement()`**：
```
if ( 条件表达式 ) 语句 [else 语句]
```

**`parseExpression()`**：处理逗号表达式。

#### 3.7.4 语法制导翻译（SDT）

Parser 在解析过程中**同步构造 AST 节点**，并调用语义分析器注册符号。例如：

```cpp
// 解析变量声明时：
auto varDecl = Arena::make<VarDecl>(NK_VarDecl);
varDecl->setName(identifier);
varDecl->setType(qualType);
m_semAnalyzer->insert(varDecl);   // 注册到符号表

// 解析初始化表达式时：
varDecl->setInitExpr(initExpr);
```

### 3.8 语义分析（Sema）

**目录**：`src/frontend/semantic/`  
**文件**：`symbol.h/cpp`、`analyzer.h/cpp`

#### 3.8.1 Symbol 类

```cpp
class Symbol {
    enum NameSpace { LABEL, RECORD, MEMBER, NORMAL };

    std::string m_name;             // 标识符名称
    QualType m_type;                // 符号类型
    NameSpace m_nameSpace;          // 所属名称空间
    Scope* m_scope;                 // 所属作用域
};
```

**名称空间**：C 语言有四个独立的名称空间：
- `LABEL`：goto 标签
- `RECORD`：struct/union/enum 的标签名
- `MEMBER`：struct/union 的成员
- `NORMAL`：普通变量、函数、typedef 名

同一作用域内不同名称空间的标识符可以同名。

#### 3.8.2 Scope 链式作用域

```cpp
class Scope {
    enum ScopeType { File, Function, Block, FunctionPrototype };
    ScopeType m_type;
    Scope* m_parent;
    std::unordered_map<std::string, Symbol*> m_symbols;
    std::unordered_map<std::string, Symbol*> m_symbolTags;  // 标签名称空间
};
```

**符号查找算法**（链式查找）：
```
lookup(name, namespace):
  current = this
  while current:
    key = getTag(namespace) + name
    if key in current.m_symbols:
      return current.m_symbols[key]
    current = current.m_parent
  return nullptr  // 未找到
```

**符号插入**：
```
insert(symbol):
  key = getTag(symbol.namespace) + symbol.name
  if key in m_symbols:
    error("重复声明")
  else:
    m_symbols[key] = symbol
```

#### 3.8.3 SymbolTableContext

```cpp
class SymbolTableContext {
    Scope* m_globalScope;           // 全局作用域
    Scope* m_currentScope;          // 当前作用域
    void pushScope(Scope::ScopeType);   // 进入新作用域
    void popScope();                    // 退出作用域
    Symbol* lookup(const std::string&, Symbol::NameSpace);
    bool insert(Symbol*);
};
```

#### 3.8.4 SemaAnalyzer — AST 后处理语义分析

**文件**：`analyzer.h/cpp`

`SemaAnalyzer` 在解析完成后作为 **AST 后处理 Pass** 执行，继承 `ASTVisitor` 遍历整棵 AST 完成类型检查、隐式转换和上下文验证。

```cpp
class SemaAnalyzer : public ASTVisitor {
public:
    SemaAnalyzer(DiagnosticEngine* diag, SymbolTableContext* ctx);

    // 入口：分析整个翻译单元
    void run(TranslationUnitDecl* tu);

    // ASTVisitor 全部 41 个 visit 方法...
    std::any visit(FunctionDecl*);
    std::any visit(BinaryOpExpr*);
    std::any visit(ReturnStmt*);
    // ...

    // 类型工具
    bool isLValue(Expr* e);
    bool isModifiableLValue(Expr* e);
    QualType getIntegerPromotionType(QualType ty);
    QualType getCommonRealType(QualType t1, QualType t2);
    bool isTypeCompatible(QualType src, QualType dst);

    // 隐式转换
    Expr* insertImplicitConversion(Expr*, QualType, CastExpr::CastKind);
    void applyUsualArithmeticConversions(Expr*& lhs, Expr*& rhs);

private:
    DiagnosticEngine* m_diag;       // 诊断引擎
    SymbolTableContext* m_ctx;      // 符号表
    std::string m_currentFuncName;  // 当前函数（错误报告用）
    QualType m_currentRetType;      // 当前函数返回类型
    int m_loopDepth;                // 循环深度（break/continue检查）
    int m_switchDepth;              // switch深度（case/default检查）
};
```

#### 3.8.5 语义分析规则

**类型推断**：为每个 Expr 节点计算并设置 `QualType`：

| 表达式类型 | 推断规则 |
|-----------|---------|
| IntegerLiteral | Parser 设置为 long long |
| FloatingLiteral | Parser 设置为 double |
| DeclRefExpr | 从符号表获取，数组类型返回指针（衰减） |
| BinaryOpExpr(算术) | 执行常用算术转换，结果类型为公共类型 |
| BinaryOpExpr(比较) | 常用算术转换，结果类型为 int |
| BinaryOpExpr(赋值) | 检查左值可修改性，执行赋值转换 |
| BinaryOpExpr(逻辑 &&\|\|) | 结果类型为 int |
| UnaryOpExpr(*) | 解引用，返回所指数组类型 |
| UnaryOpExpr(&) | 返回 PointerType |
| UnaryOpExpr(!) | 结果类型为 int |
| CallExpr | 函数返回类型 |
| ArraySubscriptExpr | 数组元素类型 |

**隐式类型转换**（C99 标准）：

| 标准 | 触发条件 | CastKind |
|------|---------|----------|
| 6.3.1.1 整数提升 | char/short → int | CK_ImplicitPromotion |
| 6.3.1.8 常用算术转换 | 二元操作数类型不同 | CK_Coercion |
| 6.5.16 赋值转换 | = 右侧转左侧类型 | CK_AssignmentConversion |
| 6.5.2.2 参数传递 | 函数实参转形参类型 | CK_FunctionArgPromotion |
| 6.3.2.1 数组→指针 | 数组用于值上下文 | CK_ArrayToPointerDecay |

**上下文验证**：

| 检查项 | 规则 | 错误示例 |
|-------|------|---------|
| return 类型 | 匹配函数返回类型 | `void f() { return 1; }` |
| break/continue | 必须在循环或 switch 内 | `if (x) { break; }` |
| case/default | 必须在 switch 内 | `if (x) { case 1: ... }` |
| 赋值目标 | 必须是可修改左值 | `const int x; x = 1;` |
| 条件类型 | 必须是标量类型 | `if (struct_var) ...` |
| 函数参数个数 | 匹配形参声明 | `f(1, 2)` 但 `f` 只声明 1 个参数 |

#### 3.8.6 语义分析集成流程

```
Parser::run(infile):
  ├── 词法分析 (Scanner)
  ├── 语法分析 + AST 构建 + 符号表注册 (Parser)
  └── 语义分析 (SemaAnalyzer::run)    ← 新增的后处理 Pass
        ├── visit(TranslationUnitDecl)
        │   ├── visit(FunctionDecl)     → 保存返回类型
        │   │   └── visit(CompoundStmt) → 遍历函数体
        │   │       ├── visit(ReturnStmt)   → 检查返回类型匹配
        │   │       ├── visit(WhileStmt)    → ++loopDepth, 检查条件
        │   │       ├── visit(BreakStmt)    → 验证 loopDepth > 0
        │   │       ├── visit(IfStmt)       → 检查条件为标量
        │   │       ├── visit(BinaryOpExpr) → 类型推断 + 隐式转换
        │   │       └── ...
        │   └── visit(VarDecl)      → 检查初始化类型兼容
        │
        └── 诊断输出: 若 hasErrors(), 抛出 CCError 终止编译
```

---

## 4. MiddleEnd 中端（SSA IR）

**目录**：`src/middleend/ir/` + `optimizer/`  
**核心文件**：`usedef.h/cpp`、`instruction.h`、`block.h/cpp`、`function.h/cpp`、`module.h/cpp`、`irbuilder.h/cpp`、`pass.h/cpp`、`dominatetree.h/cpp`、`mem2reg.h/cpp`

### 4.1 SSA IR 核心：Use-Def 链

#### 4.1.1 三核心类关系

```
Value (抽象基类，所有"值"的基类)
├── User (使用其他值的实体，继承 Value)
│   ├── Instruction (指令)
│   │   ├── BaseInst → AllocaInst, LoadInst, StoreInst, GepInst
│   │   ├── BinaryInst (所有二元运算)
│   │   ├── MemInst (GEP)
│   │   └── TerminatorInst → BranchInst, ReturnInst
│   │   └── OtherInstInstruction → CallInst, PhiInst, CastInst
│   ├── Constant (常量)
│   │   ├── ConstantInt
│   │   └── ConstantFloat
│   └── GlobalValue
│       ├── Function
│       └── GlobalVariable
└── Argument (函数参数)
```

**Use — Use-Def 链的边**：
```
User ──m_operands──→ Value (User 通过 operands 使用 Value)
Value ──m_uses──────→ Use(User) (Value 记录被哪些 User 使用)
```

```cpp
class Use {
    Value* m_value;       // 被使用的值
    User*  m_user;        // 使用者
};

class Value {
    std::string m_name;
    QualType m_type;
    std::list<Use> m_uses;          // 所有使用此 Value 的 Use
    int m_vreg = -1;                // 虚拟寄存器编号
    // Use 管理
    void addUse(User*);
    void removeUse(User*);
    void replaceAllUseWith(Value*);  // 核心 SSA 替换
};

class User : public Value {
    std::vector<Value*> m_operands;  // 操作数列表
    void setOperand(unsigned, Value*);  // 自动维护 Use 链
    void addOperand(Value*);
};
```

**Use 链自动维护**：`setOperand(i, newVal)` 自动调用旧值的 `removeUse(this)` 和新值的 `addUse(this)`，保证一致性。

**`replaceAllUseWith(val)`**：遍历 `m_uses`，将所有引用替换为新值（SSA 构造的核心操作）。

#### 4.1.2 指令操作码

`INST_MACROS_TABLE` 定义所有指令码（53 种）：

```
Alignof(0), Ret(1), Branch(2), Add(3), Sub(4), Mul(5), Div(6), Mod(7),
Shl(8), Lshr(9), Ashr(10), And(11), Or(12), Xor(13),
Eq(14), Ne(15), Lt(16), Le(17), Gt(18), Ge(19), Ult(20), Ule(21), Ugt(22), Uge(23),
LogicalAnd(24), LogicalOr(25), FAdd(26), FSub(27), FMul(28), FDiv(29),
FEq(30), FNe(31), FLt(32), FLe(33), FGt(34), FGe(35),
Assign(36), AddAssign(37), SubAssign(38), MulAssign(39), DivAssign(40),
Alloca(41), Load(42), Store(43), Gep(44), Phi(45), Call(46),
Zext(47), Sext(48), Trunc(49), FP2SI(50), SI2FP(51), Max(52), Min(53)
```

### 4.2 容器结构：Module → Function → BasicBlock

```
Module (编译单元)
├── Function 列表
│   ├── BasicBlock 列表 (entry, if.then, if.else, if.end, exit 等)
│   │   ├── AllocaInst 列表 (提前到 entry block)
│   │   └── Instruction 列表 (Load, Store, Binary, Branch, Return 等)
│   ├── Argument 列表
│   ├── 局部变量地址映射
│   └── Break/Continue 栈
├── GlobalVariable 列表
└── 全局声明地址映射
```

#### 4.2.1 Module

```cpp
class Module : public GlobalValue {
    Function* m_curFunction;                      // 当前函数
    std::vector<Function*> m_functions;            // 函数列表
    std::vector<GlobalVariable*> m_globalVars;     // 全局变量
    std::map<NamedDecl*, Value*> m_globalDeclAddr; // 全局声明地址映射

    Function* createFunctionIR(QualType, const string&);
    GlobalVariable* createGlobalVar(...);
    void setCurFunction(Function*);
    Function* getCurFunction();
};
```

#### 4.2.2 Function

```cpp
class Function : public GlobalValue {
    BasicBlock* m_curBlock;                           // 当前插入点
    BasicBlock* m_entryBlock;                         // entry 块
    BasicBlock* m_returnBlock;                        // exit 块
    std::vector<BasicBlock*> m_basicBlocks;            // 所有基本块
    std::vector<Argument*> m_arguments;                // 参数列表
    AllocaInst* m_returnAddr;                          // 返回值地址
    std::stack<std::pair<BasicBlock*, BasicBlock*>> m_breakContStack; // break/continue 栈
    std::map<NamedDecl*, Value*> m_localDeclAddr;      // 局部变量地址映射

    // 基本块管理
    BasicBlock* createAndInsertBlock(const string& name);
    void emitBlock(BasicBlock*);       // 切换到指定块
    void emitBranch(BasicBlock*);      // 在当前块末尾插入无条件跳转
    BasicBlock* getInsertBlock();

    // 控制流管理
    void pushBCStack(BasicBlock* breakBB, BasicBlock* contBB);
    void popBCStack();
    std::pair<BasicBlock*, BasicBlock*> getBCBlock();

    // 局部变量
    void addLocalDeclAddr(NamedDecl*, Value*);
    Value* getLocalDeclAddr(NamedDecl*);
};
```

**`emitBlock(block)` 逻辑**：
```
1. 清空当前块的插入点标记（如果有上一块）
2. 设置 m_curBlock = block
3. 如果上一块没有终止指令（Branch/Return），自动插入无条件跳转到新块
```

#### 4.2.3 BasicBlock

```cpp
class BasicBlock : public Value {
    std::vector<AllocaInst*> m_allocaInstructions;   // alloca 指令
    std::list<Instruction*> m_instructions;           // 普通指令
    std::list<Instruction*> getInsts();               // 合并返回所有指令
    std::vector<BasicBlock*> m_predecessors;          // 前驱块
    std::vector<BasicBlock*> m_successors;            // 后继块
    BasicBlock* m_idom;                               // 直接支配者
    std::vector<BasicBlock*> m_domChildren;           // 支配子树
    std::vector<BasicBlock*> m_domFrontier;           // 支配边界

    // 指令工厂方法
    AllocaInst* createAlloca(QualType);
    LoadInst* createLoad(Value* addr);
    StoreInst* createStore(Value* val, Value* addr);
    BinaryInst* createBinary(Instruction::OpCode, Value*, Value*);
    BranchInst* createBr(BasicBlock*);                 // 无条件
    BranchInst* createBr(Value* cond, BasicBlock*, BasicBlock*); // 条件
    ReturnInst* createReturn(QualType, Value*);
    CallInst* createCall(...);

    void addInst(Instruction*);
    void removeInst(Instruction*);
    void addAllocaInst(AllocaInst*);
    void removeAllocInst(AllocaInst*);
};
```

**指令工厂方法均会自动将指令加入基本块的指令列表**。

#### 4.2.4 IR 指令类详解

**BaseInst（基础指令）**—— `inst/baseinst.h`：

```cpp
class AllocaInst : public BaseInst {
    // 在栈上分配空间，返回地址。type 为分配的类型大小。
    // 提前到 entry block。
};

class LoadInst : public BaseInst {
    // 从地址加载值：val = *addr
    // operand[0] = 地址
    Value* getAddr();
};

class StoreInst : public BaseInst {
    // 存储值到地址：*addr = val
    // operand[0] = 值, operand[1] = 地址
    Value* getValue();
    Value* getAddr();
};
```

**BinaryInst（二元运算指令）**—— `inst/binaryinst.h`：

```cpp
class BinaryInst : public BaseInst {
    // operand[0] = lhs, operand[1] = rhs
    // opcode 决定运算类型（Add, Sub, Mul, Div, Mod, Shl, ...）
    // 支持整型和浮点运算
};
```

**TerminatorInst（终止指令）**—— `inst/terminator.h`：

```cpp
class BranchInst : public TerminatorInst {
    bool isUnconditional();          // 是否无条件跳转
    Value* getCond();                // 条件分支的条件值
    BasicBlock* getThenBlock();
    BasicBlock* getElseBlock();

    // 无条件分支：operand[0] = BasicBlock*
    // 条件分支：operand[0] = cond, operand[1] = then, operand[2] = else
};

class ReturnInst : public TerminatorInst {
    // operand[0] = 返回值（void 函数无 operand）
    Value* getReturnValue();
};
```

**OtherInstInstruction**—— `inst/otherinst.h`：

```cpp
class CallInst : public OtherInstInstruction {
    void addArg(Value*);              // 添加参数
    std::vector<Value*> getArgs();
    Value* getCallee();
};

class PhiInst : public OtherInstInstruction {
    void addIncoming(Value* val, BasicBlock* bb);  // 添加 incoming 路径
    void setValue(Value* var);        // 设置原始关联变量
    Value* getValue();
};

class CastInst : public OtherInstInstruction {
    // 类型转换指令（Zext, Sext, Trunc 等）
};
```

### 4.3 IRBuilder — AST 到 SSA IR 的翻译

#### 4.3.1 整体设计

`IRBuilder` 继承自 `ASTVisitor`，为每个 AST 节点类型实现 `visit` 方法，翻译生成 SSA IR 指令。

**关键辅助函数**：

```cpp
// 判断 Value 是否为地址类型（需要 lvalue-to-rvalue 转换）
static bool isAddressValue(Value* val) {
    return dynamic_cast<AllocaInst*>(val) || dynamic_cast<GlobalVariable*>(val);
}

// 如果值是地址，执行 Load 转换为右值
static Value* ensureRValue(BasicBlock* block, Value* val) {
    if (isAddressValue(val)) return block->createLoad(val);
    return val;
}
```

**类型擦除机制**（`makeAny` / `anyPtr`）：

```cpp
template<typename Base>
std::any makeAny(Base* node) {
    auto ptr = std::shared_ptr<Base>(node, ArenaDeleter());
    return std::any(ptr);
}

template<typename Base>
Base* anyPtr(std::any val) {
    auto node = std::any_cast<std::shared_ptr<Base>>(val);
    return node ? node.get() : nullptr;
}
```

`ArenaDeleter` 不做任何操作（`void operator()(void*) const noexcept {}`），因为内存由 Arena 统一管理。

#### 4.3.2 函数翻译流程

**`startFunction(FunctionDecl*)`**：
```
1. 创建 Function IR 对象
2. 创建 entry 块和 exit 块
3. 切换到 entry 块作为插入点
4. 分配返回地址 alloca（非 void 函数）
5. 发射函数参数（createAlloca + createStore）
6. 返回 Function 指针
```

**`endFunction(Function*)`**：
```
1. 切换到 exit 块
2. 从 retAddr 加载返回值
3. 创建 ReturnInst 指令
4. 清空模块的当前函数指针
```

#### 4.3.3 局部变量翻译

```cpp
// visit(VarDecl):
1. 计算初始化表达式 → varVal（如果是立即数则是 ConstantInt）
2. 在 entry 块创建 AllocaInst → varAddr
3. 注册局部变量地址映射: func->addLocalDeclAddr(var, varAddr)
4. 如果 varVal 有效: curBlock->createStore(varVal, varAddr)
5. 返回 varAddr（地址）
```

#### 4.3.4 表达式翻译

**`visit(DeclRefExpr*)`**：
```
返回该变量的地址（AllocaInst*），由调用者决定是否加载为右值。
```

**`visit(BinaryOpExpr*)`**：
```
1. 计算左右子表达式 → lval, rval
2. 调用 ensureRValue 确保左右值为右值（非赋值操作）
3. 根据操作符创建对应的 BinaryInst
4. 赋值操作：createStore(rval, lval)
5. 复合赋值操作：先 Load 再计算再 Store
```

**`visit(UnaryOpExpr*)`**：
```
取负(-x): 0 - x
逻辑非(!x): x == 0
按位非(~x): x xor -1
前置++/--: Load → 计算 → Store → 返回新值
后置++/--: Load → 保存旧值 → 计算 → Store → 返回旧值
取地址(&x): 直接返回 x 的地址
解引用(*x): Load(addr)
```

#### 4.3.5 控制流翻译

**`visit(IfStmt*)`**：
```
1. 创建 if.then, if.else, if.end 三个基本块
2. 计算条件表达式 → cond (Value*)
3. 在当前块末尾插入条件分支: createBr(cond, ifthen, ifelse)
4. 切换到 ifthen → 解析 then 语句 → 无条件跳转到 ifend
5. 切换到 ifelse（如果有）→ 解析 else 语句 → 无条件跳转到 ifend
6. 切换到 ifend
```

**`visit(WhileStmt*)`**：
```
1. 创建 while.cond, while.body, while.exit 三个基本块
2. 无条件跳转到 while.cond
3. while.cond: 计算条件 → createBr(cond, body, exit)
4. pushBCStack(exit, cond)  // break→exit, continue→cond
5. while.body: 解析循环体 → 无条件跳转回 while.cond
6. popBCStack()
7. 切换到 while.exit
```

**`visit(ForStmt*)`**：
```
1. 解析初始化表达式
2. 创建 for.cond, for.body, for.update, for.exit 四个基本块
3. 无条件跳转到 for.cond
4. for.cond: 计算条件（或无条件）→ createBr
5. pushBCStack(exit, update)
6. for.body: 解析循环体 → 跳转到 for.update
7. for.update: 解析更新表达式 → 跳转到 for.cond
8. popBCStack()
9. 切换到 for.exit
```

**`visit(ReturnStmt*)`**：
```
1. 计算返回值表达式 → val
2. 如果 val 是地址，调用 ensureRValue 加载为值
3. createStore(val, retAddr)
4. 切换到 exit 块
5. 清空插入点（后续代码为不可达代码）
```

**`visit(BreakStmt*)` / `visit(ContinueStmt*)`**：
```
从 BC 栈获取目标块（break→exit, continue→cond），切换到该块。
```

### 4.4 优化器

#### 4.4.1 Pass 框架

```cpp
class Pass {
    virtual string getName() = 0;
    virtual bool isAnalysis() { return false; }
    virtual bool isModulePass() = 0;
    virtual bool isFunctionPass() = 0;
    virtual bool isBasicBlockPass() = 0;
    virtual bool runOnModule(Module*) = 0;
    virtual bool runOnFunction(Function*) = 0;
    virtual bool runOnBasicBlock(BasicBlock*) = 0;
};

class ModulePass : public Pass { /* isModulePass=true */ };
class FunctionPass : public Pass { /* isFunctionPass=true */ };
class BasicBlockPass : public Pass { /* isBasicBlockPass=true */ };

class PassManager {
    vector<Pass*> m_passVec;
    void addPass(Pass*);
    void run(Module*);
};
```

#### 4.4.2 支配树算法（DominatorTree）

**背景**：在 SSA 形式中，Phi 节点的插入位置由支配边界决定。支配树是计算支配边界的基础。

**算法输入**：函数的 CFG（控制流图）。  
**算法输出**：每个基本块的直接支配者（idom）和支配边界。

**计算过程（迭代数据流算法）**：

```
阶段 1: 计算逆后序（Reverse PostOrder）

getPostOrder(entry):
  DFS 遍历 CFG，每次回溯时记录节点
  得到 m_postOrder（后序）→ reverse → 逆后序

阶段 2: 迭代计算 idom（LCA 算法，类似 Lengauer-Tarjan 简化版）

createIdom(func):
  1. entry.idom = entry（自环）
  2. 对所有其他节点，初始 idom = nullptr
  3. repeat:
     for 每个节点（逆后序）:
       for 每个前驱:
         找到第一个 idom 非空的作为 newIdom
       for 其余前驱:
         newIdom = intersect(newIdom, preNode)
       if idom[node] != newIdom:
         idom[node] = newIdom; changed = true
  4. 直到收敛

intersect(bb1, bb2):  // LCA 查找
  while bb1 != bb2:
    while bb2int[bb1] < bb2int[bb2]:  // bb1 深度更大
      bb1 = idom[bb1]                   // 上移
    while bb2int[bb1] > bb2int[bb2]:
      bb2 = idom[bb2]
  return bb1

阶段 3: 构建支配树
  for 每个节点:
    node.setIdom(idom[node])
    idom[node].addDomChildren(node)

阶段 4: 计算支配边界

createIdomFront(func):
  for 每个节点:
    if 前驱数量 < 2: continue
    for 每个前驱:
      runner = preNode
      while runner != node.getIdom():
        runner.addDomFrontier(node)
        runner = runner.getIdom()
```

**支配边界的意义**：基本块 B 的支配边界是指所有满足"B 支配 B 的某个前驱但不支配 B 本身"的基本块的集合。在 SSA 构造中，如果变量在多个路径上被定义，则在其支配边界处需要插入 Phi 节点。

**`isIdom(bb1, bb2)`**：
沿支配树从 bb2 向上查找，如果遇到 bb1 则说明 bb1 支配 bb2。

#### 4.4.3 Mem2Reg 优化 — 从内存到 SSA 寄存器

Mem2Reg 实现了经典的 **Cytron SSA 构造算法**（1991年 PLDI 论文），将 Alloca/Load/Store 模式提升为 SSA 虚拟寄存器。

**前提**：所有 Alloca 指令已提前到函数的 entry block。

**四阶段流程**：

```
runOnModule(Module):
  for 每个函数:
    (1) promotableVars = getPromotableVars(func)
    (2) for 每个可提升变量:
         收集 defBlocks（Store 所在块）
         在 defBlocks 的支配边界插入 Phi 节点
    (3) rename(func.entryBlock)  // 重命名阶段
    (4) removeAllocas()           // 清理 Alloca
```

**阶段 1: 筛选可提升 Alloca**

```cpp
isPromotableVar(AllocaInst*):
  for 每个使用:
    if 是 LoadInst: continue（允许读取）
    if 是 StoreInst:
      if 值 == alloca 地址: return false（不允许将 alloca 本身作为值存储）
      continue
    else: return false（只允许 Load/Store 访问）
  return true
```

**阶段 2: Phi 节点插入**

```cpp
insertPhiNode(defBlocks, var):
  while defBlocks 非空:
    pop block B
    for B 的每个支配边界 D:
      if D 尚未插入 Phi:
        在 D 中创建 PhiInst(var)
        defBlocks.push(D)  // Phi 也是一处定义
```

**阶段 3: 重命名（Rename）**

```
rename(block):
  // 处理 Phi 节点：Phi 是新定义，压入值栈
  for Phi 指令:
    varStack[var].push(phi)
  
  // 处理普通指令
  for Load 指令:
    val = varStack[var].top()    // 用值栈顶替换
    replaceAllUseWith(load, val)
    delete load
  
  for Store 指令:
    varStack[var].push(val)      // 新定义入栈
    delete store
  
  // 为后继块的 Phi 提供 incoming 值
  for 后继块:
    for Phi 指令:
      val = varStack[var].top()
      phi.addIncoming(val, currentBlock)
  
  // 递归处理支配子树
  for 子节点:
    rename(child)
  
  // 回溯：弹出本块定义的所有变量
  pop varStack
```

**阶段 4: 清理**

`removeAllocas()`：删除所有已提升的 AllocaInst。

---

## 5. Backend 后端（RISC-V 代码生成）

**目录**：`src/backend/`  
**核心文件**：`backendpasses.h/cpp`、`target/risc_v/riscvframeinfo.h`、`target/risc_v/riscvtargetmachine.h`、`mir/machineinst.h/cpp`、`instSelect/instselectpass.h/cpp`、`regAlloca/linearscan.h/cpp`、`regAlloca/riscvasmemitter.h/cpp`

### 5.1 后端管线

```
BackendPassManager::run(Module):
  1. InstSelectPass (指令选择)
     for 每个 SSA Function:
       创建 MachineFunction
       遍历每个 BasicBlock → 创建 MachineBlock
       遍历每条 Instruction:
         AllocaInst  → ADDI(sp, offset)
         LoadInst    → LW(reg, addr_reg)
         StoreInst   → SW(reg, addr_reg)
         BinaryInst  → ADD/SUB/MUL/AND/OR/XOR/SLT/...
         BranchInst  → BNEZ + JAL
         ReturnInst  → MV(a0, retVal) + RET
       设置栈帧大小（16字节对齐）
       建立虚拟寄存器到物理寄存器映射
  
  2. LinearScan (寄存器分配)
     for 每个 MachineFunction:
       计算活跃区间 (computeLiveInterval)
       线性扫描分配物理寄存器
  
  3. RiscVAsmEmitter (汇编发射)
     for 每个 MachineFunction:
       发射函数标签、序言
       遍历每个 MachineBlock:
         遍历每条 MachineInst:
           发射对应的汇编文本
       发射尾声
```

### 5.2 目标机器抽象

```cpp
class TargetMachine {
    virtual string getName() = 0;          // "riscv"
    virtual int getPointerSize() = 0;      // 4 (RV32)
    virtual int getRegisterWidth() = 0;    // 32 (bits)
    virtual TargetFrameInfo* getFrameInfo() = 0;
};

class TargetFrameInfo {
    virtual vector<int> getCallerSavedRegs() = 0;
    virtual vector<int> getCalleeSavedRegs() = 0;
    virtual int getStackAlignment() = 0;     // 16 (bytes)
    virtual vector<int> getArgumentRegs() = 0;
    virtual int getReturnRegs() = 0;          // A0
    virtual int getFramePointer() = 0;        // S0
    virtual int getStackPointer() = 0;        // SP
};
```

### 5.3 RISC-V 寄存器约定

```cpp
namespace RiscvReg {
    enum {
        ZERO = 0,  RA = 1,   SP = 2,   GP = 3,   TP = 4,
        T0  = 5,   T1 = 6,   T2 = 7,
        S0  = 8,   S1 = 9,
        A0  = 10,  A1 = 11,  A2 = 12,  A3 = 13,  A4 = 14, A5 = 15,
        A6  = 16,  A7 = 17,
        S2  = 18,  S3 = 19,  S4 = 20,  S5 = 21,  S6 = 22, S7 = 23,
        S8  = 24,  S9 = 25,  S10 = 26, S11 = 27,
        T3  = 28,  T4 = 29,  T5 = 30,  T6 = 31
    };
}
```

**寄存器分类**：
| 类别 | 寄存器 | 说明 |
|------|--------|------|
| Callee-saved | S0-S11 | 被调用者保存 |
| Caller-saved | T0-T6, A0-A7 | 调用者保存 |
| 参数 | A0-A7 | 传递参数 |
| 返回值 | A0 | 返回整数/指针 |
| 栈指针 | SP | 栈帧栈顶 |
| 帧指针 | S0 (FP) | 栈帧基址 |
| 零寄存器 | ZERO | 恒为 0 |

### 5.4 栈帧布局

```
高地址（Original SP）
+---------------------+
| 调用者栈帧           |
+---------------------+  ← S0 (FP) = SP + frameSize
| 保存的 RA            |  (sp + frameSize - 4)
| 保存的 S0 (FP)       |  (sp + frameSize - 8)
| 局部变量 (allocas)    |  (sp + 0 到 sp + frameSize - 8)
|                     |
+---------------------+  ← SP
低地址
```

函数序言（Prologue）：
```asm
addi sp, sp, -frameSize   # 分配栈帧
sw ra, (frameSize-4)(sp)  # 保存返回地址
sw s0, (frameSize-8)(sp)  # 保存帧指针
addi s0, sp, frameSize    # 设置帧指针
```

函数尾声（Epilogue）：
```asm
lw ra, (frameSize-4)(sp)  # 恢复返回地址
lw s0, (frameSize-8)(sp)  # 恢复帧指针
addi sp, sp, frameSize    # 释放栈帧
ret
```

### 5.5 机器 IR（MIR）

```cpp
// 四级容器
MachineModule → MachineFunction → MachineBlock → MachineInst → MachineOperand

// 机器操作数
class MachineOperand {
    enum Kind { VirtualReg, PhysicalReg, Immediate, Label };

    Kind    m_kind;
    int     m_reg;          // 寄存器编号
    int     m_imm;          // 立即数值
    string  m_label;        // 标签名
    int     m_subRegIdx;    // 子寄存器索引
    Value*  m_value;        // 关联的 SSA Value（用于寄存器分配）
    
    static MachineOperand createVReg(Value*);     // 虚拟寄存器
    static MachineOperand createPReg(int reg);     // 物理寄存器
    static MachineOperand createImm(int val);      // 立即数
    static MachineOperand createLabel(string);     // 标签
};
```

**机器指令类型**（RiscvInstOpcode）：

```cpp
enum class RiscvInstOpcode {
    ADD, SUB, ADDI, // ... 
    LW, SW,         // 访存
    SLT, SLTU,      // 比较
    BEQZ, BNEZ, JAL, RET,  // 控制流
    MV, LI, NOP,    // 辅助
    XOR, AND, OR, SLL, SRL, SRA, XORI, ANDI, ORI, // 位运算
    MUL, DIV, REM   // 乘除
};
```

### 5.6 指令选择（InstSelectPass）

**核心翻译映射表**：

| SSA 指令 | RISC-V 指令 | 说明 |
|----------|-------------|------|
| AllocaInst | ADDI rd, sp, offset | 计算变量地址 |
| LoadInst | LW rd, offset(rs) | 加载值 |
| StoreInst | SW rs, offset(rd) | 存储值 |
| BinaryInst (Add) | ADD rd, rs1, rs2 | 加法 |
| BinaryInst (Sub) | SUB rd, rs1, rs2 | 减法 |
| BinaryInst (Mul) | MUL rd, rs1, rs2 | 乘法 |
| BinaryInst (And) | AND rd, rs1, rs2 | 按位与 |
| BinaryInst (Or) | OR rd, rs1, rs2 | 按位或 |
| BinaryInst (Xor) | XOR rd, rs1, rs2 | 按位异或 |
| BinaryInst (Shl) | SLL rd, rs1, rs2 | 左移 |
| BinaryInst (Lshr) | SRL rd, rs1, rs2 | 逻辑右移 |
| BinaryInst (Ashr) | SRA rd, rs1, rs2 | 算术右移 |
| BinaryInst (Eq) | XOR + SEQZ | 相等判断 |
| BinaryInst (Ne) | XOR + SNEZ | 不等判断 |
| BinaryInst (Lt) | SLT rd, rs1, rs2 | 有符号小于 |
| BinaryInst (Gt) | SLT rd, rs2, rs1 | 有符号大于（交换操作数） |
| BinaryInst (Le) | SLT + XORI 1 | 小于等于 |
| BinaryInst (Ge) | SLT + XORI 1 | 大于等于 |
| BranchInst (条件) | BNEZ cond, ifthen; JAL zero, ifend | 条件分支 |
| BranchInst (无条件) | JAL zero, target | 无条件跳转 |
| ReturnInst | LW rd, retAddr; MV a0, rd; RET | 返回 |

**栈偏移计算**：
```
startFunction:
  m_stackOffset = 0
  for 每个 AllocaInst:
    offset = m_stackOffset
    ADDI reg, sp, offset
    m_stackOffset += allocaSize  (向上增长)
  frameSize = max(16, m_stackOffset) 对齐到16
```

### 5.7 寄存器分配：线性扫描（LinearScan）

**算法原理**：线性扫描是图染色分配的简化版本，在单次扫描中为虚拟寄存器分配物理寄存器。

```
LinearScanAllocPass::run(MachineFunction):
  1. computeLiveInterval(func):
     - 遍历所有 MachineInst，记录每对 vreg 的首次和最后一次出现
     - 计算每个虚拟寄存器的活跃区间 [startPos, endPos]

  2. 线性扫描分配：
     按 startPos 排序的活跃区间列表 intervals:
     active = []  // 当前活跃的寄存器分配
     for each interval i:
       expireOldIntervals(i, active):  // 释放 endPos < i.startPos 的区间
       if active.size() == 物理寄存器数量:
         spillAtInterval(i, active)     // 溢出处理（简单策略：溢出最晚结束的）
       else:
         分配新的物理寄存器给 i
         active.push(i)
```

**当前实现**：实际代码使用了简化的 `vreg2preg` 映射（`linearscan.cpp:80-95`），直接为每个虚拟寄存器分配可用物理寄存器。当物理寄存器不够时，会循环使用（简单循环映射）。

### 5.8 汇编发射器（RiscvAsmEmitter）

**发射流程**：

```
emit():
  1. emitPrologue()        // 函数序言
  2. for 每个 MachineBlock:
       emitLabel()
       for 每条 MachineInst:
         emitInstruction(inst)
  3. emitEpilogue()        // 函数尾声
  4. return 汇编文本
```

**指令发射映射**（`emitInstruction` 的核心逻辑）：

```cpp
switch (inst.getOpcode()):
  case ADDI:  output += format("addi %s, %s, %d\n", rd, rs1, imm);
  case ADD:   output += format("add %s, %s, %s\n", rd, rs1, rs2);
  case SUB:   output += format("sub %s, %s, %s\n", rd, rs1, rs2);
  case LW:    output += format("lw %s, %d(%s)\n", rd, imm, rs1);
  case SW:    output += format("sw %s, %d(%s)\n", rs2, imm, rs1);
  case SLT:   output += format("slt %s, %s, %s\n", rd, rs1, rs2);
  case BNEZ:  output += format("bnez %s, %s\n", rs1, label);
  case JAL:   output += format("j %s\n", label);
  case RET:   output += "ret\n";
  case MV:    output += format("mv %s, %s\n", rd, rs1);
  case LI:    output += format("li %s, %d\n", rd, imm);
  // ... 其他指令
```

**函数序言/尾声**：
```cpp
emitPrologue():
  output += ".align 2\n"
  output += ".globl " + funcName + "\n"
  output += ".type " + funcName + ", @function\n"
  output += funcName + ":\n"
  output += "    # prologue\n"
  output += format("    addi sp, sp, -%d\n", frameSize)
  output += format("    sw ra, %d(sp)\n", frameSize - 4)
  output += format("    sw s0, %d(sp)\n", frameSize - 8)
  output += format("    addi s0, sp, %d\n", frameSize)

emitEpilogue():
  output += "    # epilogue\n"
  output += format("    lw ra, %d(sp)\n", frameSize - 4)
  output += format("    lw s0, %d(sp)\n", frameSize - 8)
  output += format("    addi sp, sp, %d\n", frameSize)
  output += "    ret\n"
  output += ".size " + funcName + ", .-" + funcName + "\n"
```

---

## 6. 编译管线总览

### 6.1 完整数据流

```
C 源码 (test.c)
    │
    ▼
[Scanner] 词法分析
    │ 一次性扫描所有 Token → TokenSequence
    ▼
[Parser] 语法分析（递归下降）
    │ 同步构造 AST 节点 + 符号表注册
    ▼
[SemaAnalyzer] 语义分析（AST 后处理 Pass）
    │ 类型推断 + 隐式转换 + 上下文验证
    │ 通过 DiagnosticEngine 收集错误/警告
    ▼
[AstCtx] { TranslationUnitDecl*, SymbolTableContext* }
    │
    ▼
[IRBuilder] AST → SSA IR
    │ 遍历 AST，发射 Alloca/Load/Store/Binary/Branch/Return 等指令
    │ 仅生成内存访问形式（Alloca+Load+Store）
    ▼
[Module] SSA IR (Function → BasicBlock → Instruction)
    │
    ▼ (可选)
[DominatorTree] 支配树分析
    │ 计算 idom + 支配边界
    ▼
[Mem2Reg] 内存→寄存器提升
    │ Phi 插入 + 重命名 → SSA 形式
    ▼
[Module] SSA IR (带 Phi 的 SSA 形式)
    │
    ▼
[InstSelectPass] 指令选择
    │ SSA IR → MachineIR (RISC-V 指令)
    ▼
[MachineModule] MIR (MachineFunction → MachineBlock → MachineInst)
    │
    ▼
[LinearScan] 寄存器分配
    │ 虚拟寄存器 → 物理寄存器 (t0-t6, s0-s11, a0-a7 等)
    ▼
[MachineModule] 分配后的 MIR
    │
    ▼
[RiscvAsmEmitter] 汇编发射
    │ 生成 RISC-V 汇编文本
    ▼
RISC-V 汇编 (test.s)
```

### 6.2 输出汇编示例

```asm
.data
.align 2
.text
.align 2
.global main
.type main, @function
main:
    # prologue
    addi sp, sp, -16          # 分配栈帧
    sw ra, 12(sp)             # 保存返回地址
    sw s0, 8(sp)              # 保存帧指针
    addi s0, sp, 16           # 设置帧指针

entry:
    addi t6, sp, 0            # retAddr = sp + 0
    addi t5, sp, 4            # x = sp + 4
    addi t4, sp, 8            # y = sp + 8
    lw t3, 0(t5)              # 加载 x
    lw s11, 0(t4)             # 加载 y
    add s10, t3, s11          # x + y
    sw s10, 0(t6)             # 存到 retAddr

exit:
    lw s9, 0(t6)              # 从 retAddr 加载
    mv a0, s9                 # 设置返回值
    ret                       # 返回

    # epilogue
    lw ra, 12(sp)             # 恢复 ra
    lw s0, 8(sp)              # 恢复 fp
    addi sp, sp, 16           # 释放栈帧
    ret
.size main, .-main
```

---

## 附录：关键算法索引

| 算法 | 位置 | 复杂度 | 说明 |
|------|------|--------|------|
| 线性分配 (Bump Alloc) | `arena.cpp` | O(1) per alloc, O(N) per release | 内存分配 |
| 递归下降解析 | `parse.cpp` | O(N) | C99 语法分析 |
| 语义分析 (ASTVisitor Pass) | `analyzer.cpp` | O(N) | 类型推断 + 隐式转换 + 上下文验证 |
| 整数提升 (6.3.1.1) | `analyzer.cpp` | O(1) | char/short → int |
| 常用算术转换 (6.3.1.8) | `analyzer.cpp` | O(1) | 二元操作公共类型推导 |
| 诊断收集与格式化输出 | `diag.cpp` | O(D) per diag | 集中式错误/警告报告 |
| Use-Def 链维护 | `usedef.h/cpp` | O(1) per operand | SSA IR 基础 |
| 支配树 (迭代 LCA) | `dominatetree.cpp` | O(N²) 最坏 | 控制流分析 |
| 支配边界 (Cytron) | `dominatetree.cpp` | O(N²) 最坏 | Phi 插入定位 |
| Mem2Reg SSA 构造 | `mem2reg.cpp` | O(N × D) | Alloca→寄存器提升 |
| 指令选择 (宏展开) | `instselectpass.cpp` | O(N) | SSA→RISC-V |
| 线性扫描寄存器分配 | `linearscan.cpp` | O(N log N) | 寄存器分配 |
| 汇编发射 | `riscvasmemitter.cpp` | O(N) | 文本生成 |

---
