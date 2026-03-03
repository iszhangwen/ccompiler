#pragma once
#include "ast.h"

class Module;
class Function;
class BasicBlock;


class IRBuilder : public ASTVisitor
{
public:
    IRBuilder();
    virtual void visit(TranslationUnitDecl* tud);
    virtual void visit(LabelDecl* ld);
    virtual void visit(ValueDecl* vd);
    virtual void visit(DeclaratorDecl* dd);
    virtual void visit(VarDecl* vd);
    virtual void visit(ParmVarDecl* pvd);
    virtual void visit(FunctionDecl* fd);
    virtual void visit(FieldDecl* fd);
    virtual void visit(EnumConstantDecl* ecd);
    virtual void visit(TypedefDecl* tnd);
    virtual void visit(EnumDecl* ed);
    virtual void visit(RecordDecl* rd);
    // 访问语句节点
    virtual void visit(LabelStmt* ls);
    virtual void visit(CaseStmt* cs);
    virtual void visit(DefaultStmt* ds);
    virtual void visit(CompoundStmt* cs);
    virtual void visit(DeclStmt* ds);
    virtual void visit(ExprStmt* es);
    virtual void visit(IfStmt* is);
    virtual void visit(SwitchStmt* ss);
    virtual void visit(WhileStmt* ws);
    virtual void visit(DoStmt* ds);
    virtual void visit(ForStmt* fs);
    virtual void visit(GotoStmt* gs);
    virtual void visit(ContinueStmt* cs);
    virtual void visit(BreakStmt* bs);
    virtual void visit(ReturnStmt* rs);
    // 访问表达式节点
    virtual void visit(IntegerLiteral* c);
    virtual void visit(FloatingLiteral* c);
    virtual void visit(CharacterLiteral* c);
    virtual void visit(StringLiteral* c);
    virtual void visit(DeclRefExpr* dre);
    virtual void visit(ParenExpr* pe);
    virtual void visit(BinaryOpExpr* boe);
    virtual void visit(ConditionalExpr* ce);
    virtual void visit(CompoundLiteralExpr* cle);
    virtual void visit(CastExpr* ce);
    virtual void visit(ArraySubscriptExpr* ase);
    virtual void visit(CallExpr* ce);
    virtual void visit(MemberExpr* me);
    virtual void visit(UnaryOpExpr* uoe);
    void dump();

private:
    // 插入点相关代码
    void setInsertPoint(std::shared_ptr<BasicBlock> bb) {m_curBlock = bb;}
    std::shared_ptr<BasicBlock> getInsertPoint() {return m_curBlock;}
    bool hasInsertPoint() const {return m_curBlock != nullptr;}
    void clearInsertPoint() {m_curBlock = nullptr;}
    
    // ssa ir生成关键是基本块的划分，
    void emitBlock(std::shared_ptr<BasicBlock> bb, bool isFinished);
    void emitBranch(std::shared_ptr<BasicBlock> bb);

private:
    std::shared_ptr<Module> m_module;
    std::shared_ptr<Function> m_curFuntion;
    std::shared_ptr<BasicBlock> m_curBlock;
};

