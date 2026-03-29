#pragma once
#include "ast.h"
#include "usedef.h"

#include <stack>
#include <unordered_map>
#include <any>

class Module;
class Function;

class IRBuilder : public ASTVisitor
{
public:
    std::any visit(TranslationUnitDecl*);
    std::any visit(LabelDecl*);
    std::any visit(ValueDecl*);
    std::any visit(DeclaratorDecl*);
    std::any visit(VarDecl*);
    std::any visit(ParmVarDecl*);
    std::any visit(FunctionDecl*);
    std::any visit(FieldDecl*);
    std::any visit(EnumConstantDecl*);
    std::any visit(TypedefDecl*);
    std::any visit(EnumDecl*);
    std::any visit(RecordDecl*);
    // 访问语句节点
    std::any visit(LabelStmt*);
    std::any visit(CaseStmt*);
    std::any visit(DefaultStmt*);
    std::any visit(CompoundStmt*);
    std::any visit(DeclStmt*);
    std::any visit(ExprStmt*);
    std::any visit(IfStmt*);
    std::any visit(SwitchStmt*);
    std::any visit(WhileStmt*);
    std::any visit(DoStmt*);
    std::any visit(ForStmt*);
    std::any visit(GotoStmt*);
    std::any visit(ContinueStmt*);
    std::any visit(BreakStmt*);
    std::any visit(ReturnStmt*);
    // 访问表达式节点
    std::any visit(IntegerLiteral*);
    std::any visit(FloatingLiteral*);
    std::any visit(CharacterLiteral*);
    std::any visit(StringLiteral*);
    std::any visit(DeclRefExpr*);
    std::any visit(ParenExpr*);
    std::any visit(BinaryOpExpr*);
    std::any visit(ConditionalExpr*);
    std::any visit(CompoundLiteralExpr*);
    std::any visit(CastExpr*);
    std::any visit(ArraySubscriptExpr*);
    std::any visit(CallExpr*);
    std::any visit(MemberExpr*);
    std::any visit(UnaryOpExpr*);

    IRBuilder();
    // 获取模块节点
    Module* getModule() {return m_module;}
    // 执行编译
    void run(AstCtx astIR);

private:
    Function* startFunction(FunctionDecl*);
    void emitFunctionParams(FunctionDecl*, Function*);
    void endFunction(Function*);

    // @brief: 自定义删除器，Arena分配的内存无delete运算符
    struct ArenaDeleter {void operator()(void*) const noexcept {}};

    // @brief: 将指针转化为std::any，类型擦除
    template <typename Base>
    std::any makeAny(Base* node) {
        // 将node存储到unique_ptr，方便从std::any还原
        auto ptr = std::shared_ptr<Base>(node, ArenaDeleter());
        return std::any(ptr);
    }
    // @brief: 支持nullptr
    template<typename Base>
    std::any makeAny() {
        auto ptr = std::shared_ptr<Base>(nullptr, ArenaDeleter());
        return std::any(ptr);
    }
    // @brief: 从std::any提取裸指针[std::any不知此多态]
    template<typename Base>
    Base* anyPtr(std::any val) {
        auto node = std::any_cast<std::shared_ptr<Base>>(val);
        if (!node) 
            return nullptr;
        return node.get();
    }
    // @brief: 从std::any提取裸指针,并转化为派生类指针
    template<typename Base, typename T>
    T* anyPtr(std::any val) {
        static_assert(std::is_base_of_v<Base, T>, "Derived must inherit from Base");
        try {
            // 尝试取出 std::shared_ptr<Base>
            auto node = std::any_cast<std::shared_ptr<Base>>(val);
            if (!node) {
                return nullptr;
            }
            return dynamic_cast<T*>(node.get());
        } catch (const std::bad_any_cast&) {
            return nullptr; 
        }
    }

private:
    Module* m_module;
    Function* m_curFuntion;
    std::unordered_map<NamedDecl*, Value*> m_nameDeclAddr;
    // ast ir上下文：符号表和unit
    AstCtx m_astCtx;
    
};

