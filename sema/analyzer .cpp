#include "sema.h"
#include <ast/decl.h>
#include <ast/expr.h>
#include <ast/stmt.h>
#include <ast/type.h>


void SemaAnalyzer::visit(BuiltinType* bt)
{
    // 处理内置类型
    if (bt) {
        // 在这里可以添加对内置类型的处理逻辑
        // 例如，记录内置类型的符号等v
        // bt->getSymbol()->setType(bt);
    }
}

void SemaAnalyzer::visit(PointerType* pt)
{
    // 处理指针类型
    if (pt) {
        // 在这里可以添加对指针类型的处理逻辑
        // 例如，记录指针类型的符号等
        // pt->getSymbol()->setType(pt);
    }
}

void SemaAnalyzer::visit(ArrayType at)
{
    // 处理数组类型
    if (at) {
        // 在这里可以添加对数组类型的处理逻辑
        // 例如，记录数组类型的符号等
        // at->getSymbol()->setType(at);
    }
}

void SemaAnalyzer::visit(FunctionType* ft)
{
    // 处理函数类型
    if (ft) {
        // 在这里可以添加对函数类型的处理逻辑
        // 例如，记录函数类型的符号等
        // ft->getSymbol()->setType(ft);
    }
}

void SemaAnalyzer::visit(RecordType* rt)
{
    // 处理记录类型
    if (rt) {
        // 在这里可以添加对记录类型的处理逻辑
        // 例如，记录记录类型的符号等
        // rt->getSymbol()->setType(rt);
    }
}

void SemaAnalyzer::visit(EnumType* et)
{
    // 处理枚举类型
    if (et) {
        // 在这里可以添加对枚举类型的处理逻辑
        // 例如，记录枚举类型的符号等
        // et->getSymbol()->setType(et);
    }
}

void SemaAnalyzer::visit(TypedefType* tt)
{
    // 处理类型定义
    if (tt) {
        // 在这里可以添加对类型定义的处理逻辑
        // 例如，记录类型定义的符号等
        // tt->getSymbol()->setType(tt);
    }
}

void SemaAnalyzer::visit(Constant* c)
{
    // 处理常量表达式
    if (c) {
        // 在这里可以添加对常量表达式的处理逻辑
        // 例如，记录常量的值等
        // c->getSymbol()->setValue(c->getValue());
    }
}

void SemaAnalyzer::visit(DeclRefExpr* dre)
{
    // 处理声明引用表达式
    if (dre) {
        // 在这里可以添加对声明引用表达式的处理逻辑
        // 例如，查找符号表中的符号等
        // dre->setSymbol(sys_->lookup(dre->getName()));
    }
}

void SemaAnalyzer::visit(ParenExpr* pe)
{
    // 处理括号表达式
    if (pe) {
        // 在这里可以添加对括号表达式的处理逻辑
        // 例如，记录括号内的表达式等
        // pe->setInnerExpr(pe->getInnerExpr());
    }
}

void SemaAnalyzer::visit(BinaryOpExpr* boe)
{
    // 处理二元操作表达式
    if (boe) {
        // 在这里可以添加对二元操作表达式的处理逻辑
        // 例如，记录操作符和操作数等
        // boe->setLeft(boe->getLeft());
        // boe->setRight(boe->getRight());
    }
}

void SemaAnalyzer::visit(ConditionalExpr* ce)
{
    // 处理条件表达式
    if (ce) {
        // 在这里可以添加对条件表达式的处理逻辑
        // 例如，记录条件和结果等
        // ce->setCondition(ce->getCondition());
        // ce->setTrueExpr(ce->getTrueExpr());
        // ce->setFalseExpr(ce->getFalseExpr());
    }
}

void SemaAnalyzer::visit(CompoundLiteralExpr* cle)
{
    // 处理复合字面量表达式
    if (cle) {
        // 在这里可以添加对复合字面量表达式的处理逻辑
        // 例如，记录复合字面量的类型和初始值等
        // cle->setType(cle->getType());
        // cle->setInitializer(cle->getInitializer());
    }
}

void SemaAnalyzer::visit(CastExpr* ce)
{
    // 处理类型转换表达式
    if (ce) {
        // 在这里可以添加对类型转换表达式的处理逻辑
        // 例如，记录转换的类型和被转换的表达式等
        // ce->setCastType(ce->getCastType());
        // ce->setSubExpr(ce->getSubExpr());
    }
}

void SemaAnalyzer::visit(ArraySubscriptExpr* ase)
{
    // 处理数组下标表达式
    if (ase) {
        // 在这里可以添加对数组下标表达式的处理逻辑
        // 例如，记录数组和下标等
        // ase->setArray(ase->getArray());
        // ase->setIndex(ase->getIndex());
    }
}

void SemaAnalyzer::visit(CallExpr* ce)
{
    // 处理函数调用表达式
    if (ce) {
        // 在这里可以添加对函数调用表达式的处理逻辑
        // 例如，记录被调用的函数和参数等
        // ce->setCallee(ce->getCallee());
        // ce->setArgs(ce->getArgs());
    }
}

void SemaAnalyzer::visit(MemberExpr* me)
{
    // 处理成员访问表达式
    if (me) {
        // 在这里可以添加对成员访问表达式的处理逻辑
        // 例如，记录结构体或类和成员等
        // me->setBase(me->getBase());
        // me->setMember(me->getMember());
    }
}

void SemaAnalyzer::visit(UnaryOpExpr* uoe)
{
    // 处理一元操作表达式
    if (uoe) {
        // 在这里可以添加对一元操作表达式的处理逻辑
        // 例如，记录操作符和操作数等
        // uoe->setOperand(uoe->getOperand());
        // uoe->setOpCode(uoe->getOpCode());
    }
}

void SemaAnalyzer::visit(TranslationUnitDecl* tud)
{
    // 处理翻译单元声明
    if (tud) {
        // 在这里可以添加对翻译单元声明的处理逻辑
        // 例如，记录全局变量和函数等
        // tud->setGlobalVars(tud->getGlobalVars());
        // tud->setFunctions(tud->getFunctions());
    }
}
void SemaAnalyzer::visit(LabelDecl* ld)
{
    // 处理标签声明
    if (ld) {
        // 在这里可以添加对标签声明的处理逻辑
        // 例如，记录标签名称和类型等
        // ld->setName(ld->getName());
        // ld->setType(ld->getType());
    }
}
void SemaAnalyzer::visit(ValueDecl* vd)
{
    // 处理值声明
    if (vd) {
        // 在这里可以添加对值声明的处理逻辑
        // 例如，记录值的名称和类型等
        // vd->setName(vd->getName());
        // vd->setType(vd->getType());
    }
}

void SemaAnalyzer::visit(DeclaratorDecl* dd)
{
    // 处理声明符声明
    if (dd) {
        // 在这里可以添加对声明符声明的处理逻辑
        // 例如，记录声明符的名称和类型等
        // dd->setName(dd->getName());
        // dd->setType(dd->getType());
    }
}

void SemaAnalyzer::visit(VarDecl* vd)
{
    // 处理变量声明
    if (vd) {
        // 在这里可以添加对变量声明的处理逻辑
        // 例如，记录变量的名称和类型等
        // vd->setName(vd->getName());
        // vd->setType(vd->getType());
    }
}
void SemaAnalyzer::visit(ParmVarDecl* pvd)
{
    // 处理参数变量声明
    if (pvd) {
        // 在这里可以添加对参数变量声明的处理逻辑
        // 例如，记录参数的名称和类型等
        // pvd->setName(pvd->getName());
        // pvd->setType(pvd->getType());
    }
}
void SemaAnalyzer::visit(FunctionDecl* fd)
{
    // 处理函数声明
    if (fd) {
        // 在这里可以添加对函数声明的处理逻辑
        // 例如，记录函数的名称、返回类型和参数等
        // fd->setName(fd->getName());
        // fd->setReturnType(fd->getReturnType());
        // fd->setParams(fd->getParams());
    }
}
void SemaAnalyzer::visit(FieldDecl* fd)
{
    // 处理字段声明
    if (fd) {
        // 在这里可以添加对字段声明的处理逻辑
        // 例如，记录字段的名称、类型和偏移量等
        // fd->setName(fd->getName());
        // fd->setType(fd->getType());
        // fd->setOffset(fd->getOffset());
    }
}

void SemaAnalyzer::visit(EnumConstantDecl* ecd)
{
    // 处理枚举常量声明
    if (ecd) {
        // 在这里可以添加对枚举常量声明的处理逻辑
        // 例如，记录枚举常量的名称和值等
        // ecd->setName(ecd->getName());
        // ecd->setValue(ecd->getValue());
    }
}

void SemaAnalyzer::visit(IndirectFieldDecl* ifd)
{
    // 处理间接字段声明
    if (ifd) {
        // 在这里可以添加对间接字段声明的处理逻辑
        // 例如，记录间接字段的名称和类型等
        // ifd->setName(ifd->getName());
        // ifd->setType(ifd->getType());
    }
}

void SemaAnalyzer::visit(TypedefNameDecl* tnd)
{
    // 处理类型定义名称声明
    if (tnd) {
        // 在这里可以添加对类型定义名称声明的处理逻辑
        // 例如，记录类型定义的名称和类型等
        // tnd->setName(tnd->getName());
        // tnd->setType(tnd->getType());
    }
}

void SemaAnalyzer::visit(EnumDecl* ed)
{
    // 处理枚举声明
    if (ed) {
        // 在这里可以添加对枚举声明的处理逻辑
        // 例如，记录枚举的名称和常量等
        // ed->setName(ed->getName());
        // ed->setConstants(ed->getConstants());
    }
}

void SemaAnalyzer::visit(RecordDecl* rd)
{
    // 处理记录声明
    if (rd) {
        // 在这里可以添加对记录声明的处理逻辑
        // 例如，记录记录的名称和字段等
        // rd->setName(rd->getName());
        // rd->setFields(rd->getFields());
    }
}

void SemaAnalyzer::visit(LabelStmt* ls)
{
    // 处理标签语句
    if (ls) {
        // 在这里可以添加对标签语句的处理逻辑
        // 例如，记录标签的名称和类型等
        // ls->setLabel(ls->getLabel());
    }
}

void SemaAnalyzer::visit(CaseStmt* cs)
{
    // 处理 case 语句
    if (cs) {
        // 在这里可以添加对 case 语句的处理逻辑
        // 例如，记录 case 的值和类型等
        // cs->setValue(cs->getValue());
    }
}

void SemaAnalyzer::visit(DefaultStmt* ds)
{
    // 处理 default 语句
    if (ds) {
        // 在这里可以添加对 default 语句的处理逻辑
        // 例如，记录 default 的类型等
        // ds->setType(ds->getType());
    }
}

void SemaAnalyzer::visit(CompoundStmt* cs)
{
    // 处理复合语句
    if (cs) {
        // 在这里可以添加对复合语句的处理逻辑
        // 例如，记录复合语句中的语句列表等
        // cs->setStatements(cs->getStatements());
    }
}

void SemaAnalyzer::visit(DeclStmt* ds)
{
    // 处理声明语句
    if (ds) {
        // 在这里可以添加对声明语句的处理逻辑
        // 例如，记录声明的变量和类型等
        // ds->setDeclarations(ds->getDeclarations());
    }
}

void SemaAnalyzer::visit(ExprStmt* es)
{
    // 处理表达式语句
    if (es) {
        // 在这里可以添加对表达式语句的处理逻辑
        // 例如，记录表达式的类型和值等
        // es->setExpression(es->getExpression());
    }
}   

void SemaAnalyzer::visit(IfStmt* is)
{
    // 处理 if 语句
    if (is) {
        // 在这里可以添加对 if 语句的处理逻辑
        // 例如，记录条件表达式和分支语句等
        // is->setCondition(is->getCondition());
        // is->setThenBranch(is->getThenBranch());
        // is->setElseBranch(is->getElseBranch());
    }
}

void SemaAnalyzer::visit(SwitchStmt* ss)
{
    // 处理 switch 语句
    if (ss) {
        // 在这里可以添加对 switch 语句的处理逻辑
        // 例如，记录条件表达式和 case 语句等
        // ss->setCondition(ss->getCondition());
        // ss->setCases(ss->getCases());
    }
}

void SemaAnalyzer::visit(WhileStmt* ws)
{
    // 处理 while 语句
    if (ws) {
        // 在这里可以添加对 while 语句的处理逻辑
        // 例如，记录条件表达式和循环体等
        // ws->setCondition(ws->getCondition());
        // ws->setBody(ws->getBody());
    }
}

void SemaAnalyzer::visit(DoStmt* ds)
{
    // 处理 do 语句
    if (ds) {
        // 在这里可以添加对 do 语句的处理逻辑
        // 例如，记录循环体和条件表达式等
        // ds->setBody(ds->getBody());
        // ds->setCondition(ds->getCondition());
    }
}

void SemaAnalyzer::visit(ForStmt* fs)
{
    // 处理 for 语句
    if (fs) {
        // 在这里可以添加对 for 语句的处理逻辑
        // 例如，记录初始化表达式、条件表达式和循环体等
        // fs->setInit(fs->getInit());
        // fs->setCondition(fs->getCondition());
        // fs->setBody(fs->getBody());
    }
}

void SemaAnalyzer::visit(GotoStmt* gs)
{
    // 处理 goto 语句
    if (gs) {
        // 在这里可以添加对 goto 语句的处理逻辑
        // 例如，记录跳转目标标签等
        // gs->setTarget(gs->getTarget());
    }
}

void SemaAnalyzer::visit(ContinueStmt* cs)
{
    // 处理 continue 语句
    if (cs) {
        // 在这里可以添加对 continue 语句的处理逻辑
        // 例如，记录循环体等
        // cs->setLoop(cs->getLoop());
    }
}

void SemaAnalyzer::visit(BreakStmt* bs)
{
    // 处理 break 语句
    if (bs) {
        // 在这里可以添加对 break 语句的处理逻辑
        // 例如，记录循环体等
        // bs->setLoop(bs->getLoop());
    }
}

void SemaAnalyzer::visit(ReturnStmt* rs)
{
    // 处理 return 语句
    if (rs) {
        // 在这里可以添加对 return 语句的处理逻辑
        // 例如，记录返回值和函数等
        // rs->setReturnValue(rs->getReturnValue());
        // rs->setFunction(rs->getFunction());
    }
}

