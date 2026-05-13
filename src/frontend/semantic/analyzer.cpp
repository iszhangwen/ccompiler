#include "analyzer.h"
#include "type.h"
#include "decl.h"
#include "stmt.h"
#include "expr.h"

using namespace ccompiler;

// ============================================================
// 构造函数
// ============================================================
SemaAnalyzer::SemaAnalyzer(DiagnosticEngine* diag, SymbolTableContext* ctx)
    : m_diag(diag)
    , m_ctx(ctx)
    , m_loopDepth(0)
    , m_switchDepth(0) {}

void SemaAnalyzer::run(TranslationUnitDecl* tu) {
    if (!tu) return;
    tu->accept(this);
}

// ============================================================
// 类型工具方法
// ============================================================

// 判断表达式是否为左值 (C99 6.3.2.1)
// 左值包括：变量名、指针解引用、数组下标
bool SemaAnalyzer::isLValue(Expr* e) {
    if (!e) return false;
    switch (e->getKind()) {
        case AstNode::NK_DeclRefExpr: {
            auto dre = static_cast<DeclRefExpr*>(e);
            auto decl = dre->getDecl();
            if (!decl) return false;
            // 函数名不是左值
            if (decl->getKind() == AstNode::NK_FunctionDecl) return false;
            // 枚举常量不是左值
            if (decl->getKind() == AstNode::NK_EnumConstantDecl) return false;
            return true;
        }
        case AstNode::NK_UnaryOperator: {
            auto uop = static_cast<UnaryOpExpr*>(e);
            return uop->getOpCode() == UnaryOpExpr::Multiplication_; // *ptr 是左值
        }
        case AstNode::NK_ArraySubscriptExpr:
            return true; // a[i] 是左值
        case AstNode::NK_MemberExpr:
            return true; // s.m 是左值
        case AstNode::NK_StringLiteral:
            return true; // 字符串字面量是左值
        default:
            return false;
    }
}

// 判断是否为可修改左值 (C99 6.5.16)
bool SemaAnalyzer::isModifiableLValue(Expr* e) {
    if (!isLValue(e)) return false;
    // 检查类型是否 const 限定
    QualType qt = e->getType();
    if (!qt.isNull() && qt.isConstQual()) return false;
    // 数组类型不可修改
    if (!qt.isNull() && qt->isArrayType()) return false;
    return true;
}

// 整数提升 (C99 6.3.1.1)
// _Bool, char, signed/unsigned char, signed/unsigned short 提升为 int
QualType SemaAnalyzer::getIntegerPromotionType(QualType ty) {
    if (ty.isNull()) return ty;
    // 如果已经是 int 或更宽，不需要提升
    if (ty->getKind() == Type::INTEGER) {
        auto it = ty.as<IntegerType>();
        if (!it) return ty;
        // bool, char, short 需要提升为 int
        if (it->isChar() || it->isShort() || ty->isBoolType()) {
            // 返回 signed int
            auto intType = m_ctx->getBuiltTypeByTS(TypeSpecifier::INT);
            return QualType(intType);
        }
        // int 或更宽的不需要提升
        return ty;
    }
    // float/double 不提升
    if (ty->isRealFloatingType()) return ty;
    return ty;
}

// 常用算术转换 (C99 6.3.1.8)
// 找到二元操作的公共实类型
QualType SemaAnalyzer::getCommonRealType(QualType t1, QualType t2) {
    if (t1.isNull() || t2.isNull()) return QualType();

    // 先对两个操作数执行整数提升
    QualType pt1 = getIntegerPromotionType(t1);
    QualType pt2 = getIntegerPromotionType(t2);

    // 如果相同则直接返回
    if (pt1 == pt2) return pt1;

    // 浮点类型优先级: long double > double > float
    if (pt1->isRealFloatingType() || pt2->isRealFloatingType()) {
        if (pt1->isRealFloatingType() && pt2->isRealFloatingType()) {
            auto f1 = pt1.as<RealFloatingType>();
            auto f2 = pt2.as<RealFloatingType>();
            if (!f1 || !f2) return pt1;
            // 取精度更高的类型
            if (f1->isLongDouble() || f2->isLongDouble())
                return QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::LONG | TypeSpecifier::DOUBLE));
            if (f1->isDouble() || f2->isDouble())
                return QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::DOUBLE));
            return QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::FLOAT));
        }
        // 一个浮点一个整数: 转为浮点
        if (pt1->isRealFloatingType()) return pt1;
        return pt2;
    }

    // 两个都是整数类型
    if (pt1->isIntegerType() && pt2->isIntegerType()) {
        auto i1 = pt1.as<IntegerType>();
        auto i2 = pt2.as<IntegerType>();
        if (!i1 || !i2) return pt1;

        // 如果符号相同，取宽度更大的
        if (i1->isSigned() == i2->isSigned()) {
            // 按宽度排序: BYTE < SHORT < NORMAL < LONG < LONG2
            auto getWidthRank = [](IntegerType* it) -> int {
                if (it->isShort()) return 1;
                if (it->isNormal()) return 2;
                if (it->isLong()) return 3;
                if (it->isLongLong()) return 4;
                return 0;
            };
            int r1 = getWidthRank(i1);
            int r2 = getWidthRank(i2);
            if (r1 >= r2) return pt1;
            return pt2;
        }

        // 符号不同: 无符号类型能容纳有符号类型的所有值则用无符号，否则用有符号的更大类型
        if (i1->isUnSigned()) {
            if (i2->isNormal()) return pt1; // unsigned int 胜过 int
            if (i2->isLong()) return pt2;    // long 胜过 unsigned int
            if (i2->isLongLong()) return pt2; // long long 胜过 unsigned int
            return pt1;
        }
        // i1 isSigned, i2 isUnsigned
        {
            if (i1->isNormal()) return pt2; // unsigned int 胜过 int
            if (i1->isLong()) return pt1;    // long 胜过 unsigned int
            if (i1->isLongLong()) return pt1; // long long 胜过 unsigned int
            return pt2;
        }
    }

    // 默认返回第一个
    return pt1;
}

// 类型兼容性检查（用于赋值兼容性）
bool SemaAnalyzer::isTypeCompatible(QualType src, QualType dst) {
    if (src.isNull() || dst.isNull()) return true;

    // void 可以接受任何表达式作为副作用
    if (dst->isVoidType()) return true;

    // 相同类型兼容
    if (src == dst) return true;

    // 数值类型间的隐式转换
    if (src->isArithmeticType() && dst->isArithmeticType()) return true;

    // 指针类型兼容
    if (src->isPointerType() && dst->isPointerType()) {
        // void* <-> any pointer
        auto srcPointee = src.as<PointerType>()->getPointeeType();
        auto dstPointee = dst.as<PointerType>()->getPointeeType();
        if (srcPointee->isVoidType() || dstPointee->isVoidType()) return true;
        return srcPointee == dstPointee;
    }

    // 指针和整数（0 可以转任意指针）
    if (dst->isPointerType() && src->isIntegerType()) return true;
    if (src->isPointerType() && dst->isIntegerType()) return true;

    // 数组到指针衰减兼容
    if (src->isArrayType() && dst->isPointerType()) {
        auto arrType = src.as<ArrayType>();
        auto ptrType = dst.as<PointerType>();
        if (arrType && ptrType) {
            return isTypeCompatible(arrType->getQualType(), ptrType->getPointeeType());
        }
    }

    return false;
}

// 插入隐式转换
Expr* SemaAnalyzer::insertImplicitConversion(Expr* expr, QualType targetType, CastExpr::CastKind kind) {
    if (!expr) return nullptr;
    auto cast = Arena::make<CastExpr>();
    cast->setCastExpr(expr);
    cast->setCastKind(kind);
    cast->setType(targetType);
    return cast;
}

// 常用算术转换应用于二元操作数
void SemaAnalyzer::applyUsualArithmeticConversions(Expr*& lhs, Expr*& rhs) {
    if (!lhs || !rhs) return;
    QualType commonType = getCommonRealType(lhs->getType(), rhs->getType());
    if (commonType.isNull()) return;

    // 如果 lhs 类型不是 commonType，插入隐式转换
    if (lhs->getType() != commonType) {
        lhs = insertImplicitConversion(lhs, commonType, CastExpr::CK_Coercion);
    }
    // 如果 rhs 类型不是 commonType，插入隐式转换
    if (rhs->getType() != commonType) {
        rhs = insertImplicitConversion(rhs, commonType, CastExpr::CK_Coercion);
    }
}

// 应用整数提升
Expr* SemaAnalyzer::applyIntegerPromotions(Expr* expr) {
    if (!expr) return nullptr;
    QualType promoted = getIntegerPromotionType(expr->getType());
    if (promoted.isNull()) return expr;
    if (promoted == expr->getType()) return expr;
    return insertImplicitConversion(expr, promoted, CastExpr::CK_ImplicitPromotion);
}

// ============================================================
// 声明节点的 visit 方法
// ============================================================

std::any SemaAnalyzer::visit(TranslationUnitDecl* tu) {
    if (!tu) return {};
    m_currentFuncName.clear();
    m_currentRetType = QualType();
    for (auto decl : tu->getDecls()) {
        if (decl) decl->accept(this);
    }
    return {};
}

std::any SemaAnalyzer::visit(LabelDecl*) { return {}; }
std::any SemaAnalyzer::visit(ValueDecl*) { return {}; }
std::any SemaAnalyzer::visit(DeclaratorDecl*) { return {}; }

std::any SemaAnalyzer::visit(VarDecl* var) {
    if (!var) return {};
    // 检查初始化表达式类型是否与变量类型兼容
    Expr* init = var->getInitExpr();
    if (init) {
        QualType initType = std::any_cast<QualType>(init->accept(this));
        QualType varType = var->getType();
        if (!initType.isNull() && !varType.isNull()) {
            if (!isTypeCompatible(initType, varType)) {
                m_diag->warning("initialization of '" + varType->getName()
                    + "' with type '" + initType->getName()
                    + "' may lose precision in function '" + m_currentFuncName + "'");
            }
            // 插入隐式赋值转换
            if (initType != varType && varType->isArithmeticType()) {
                var->setInitExpr(insertImplicitConversion(init, varType, CastExpr::CK_AssignmentConversion));
            }
        }
    }
    return {};
}

std::any SemaAnalyzer::visit(ParmVarDecl*) { return {}; }

std::any SemaAnalyzer::visit(FunctionDecl* func) {
    if (!func) return {};
    // 保存当前函数上下文
    std::string prevFunc = m_currentFuncName;
    QualType prevRet = m_currentRetType;

    m_currentFuncName = func->getName();

    // 从 FunctionType 获取返回类型
    QualType funcType = func->getType();
    if (!funcType.isNull() && funcType->isFunctionType()) {
        auto ft = funcType.as<FunctionType>();
        if (ft) {
            m_currentRetType = ft->getRetType();
        }
    }

    // 访问函数体
    CompoundStmt* body = func->getBody();
    if (body) {
        body->accept(this);
    }

    // 恢复上下文
    m_currentFuncName = prevFunc;
    m_currentRetType = prevRet;
    return {};
}

std::any SemaAnalyzer::visit(FieldDecl*) { return {}; }
std::any SemaAnalyzer::visit(EnumConstantDecl*) { return {}; }
std::any SemaAnalyzer::visit(TypedefDecl*) { return {}; }
std::any SemaAnalyzer::visit(EnumDecl*) { return {}; }
std::any SemaAnalyzer::visit(RecordDecl*) { return {}; }

// ============================================================
// 语句节点的 visit 方法
// ============================================================

std::any SemaAnalyzer::visit(LabelStmt* stmt) {
    if (!stmt) return {};
    // 标签语句 - 验证标签和继续访问子语句
    if (stmt->getStmt()) stmt->getStmt()->accept(this);
    return {};
}

std::any SemaAnalyzer::visit(CaseStmt* stmt) {
    if (!stmt) return {};
    if (m_switchDepth <= 0) {
        m_diag->error("'case' label outside switch statement in function '" + m_currentFuncName + "'");
    }
    // 检查 case 表达式是否为整数常量
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isIntegerType()) {
            m_diag->error("'case' expression must be an integer constant in function '" + m_currentFuncName + "'");
        }
    }
    if (stmt->getBody()) stmt->getBody()->accept(this);
    return {};
}

std::any SemaAnalyzer::visit(DefaultStmt* stmt) {
    if (!stmt) return {};
    if (m_switchDepth <= 0) {
        m_diag->error("'default' label outside switch statement in function '" + m_currentFuncName + "'");
    }
    if (stmt->getBody()) stmt->getBody()->accept(this);
    return {};
}

std::any SemaAnalyzer::visit(CompoundStmt* stmt) {
    if (!stmt) return {};
    for (auto s : stmt->getStmts()) {
        if (s) s->accept(this);
    }
    return {};
}

std::any SemaAnalyzer::visit(DeclStmt* stmt) {
    if (!stmt) return {};
    if (stmt->getDecl()) stmt->getDecl()->accept(this);
    return {};
}

std::any SemaAnalyzer::visit(ExprStmt* stmt) {
    if (!stmt) return {};
    if (stmt->getExpr()) {
        stmt->getExpr()->accept(this);
    }
    return {};
}

std::any SemaAnalyzer::visit(IfStmt* stmt) {
    if (!stmt) return {};
    // 检查条件是否为标量类型
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isScalarType()) {
            m_diag->warning("if condition is not a scalar type in function '" + m_currentFuncName + "'");
        }
    }
    if (stmt->getThen()) stmt->getThen()->accept(this);
    if (stmt->getElse()) stmt->getElse()->accept(this);
    return {};
}

std::any SemaAnalyzer::visit(SwitchStmt* stmt) {
    if (!stmt) return {};
    // 检查条件是否为整数类型
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isIntegerType()) {
            m_diag->error("switch condition must be an integer type in function '" + m_currentFuncName + "'");
        }
    }
    ++m_switchDepth;
    if (stmt->getBody()) stmt->getBody()->accept(this);
    --m_switchDepth;
    return {};
}

std::any SemaAnalyzer::visit(WhileStmt* stmt) {
    if (!stmt) return {};
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isScalarType()) {
            m_diag->warning("while condition is not a scalar type in function '" + m_currentFuncName + "'");
        }
    }
    ++m_loopDepth;
    if (stmt->getBody()) stmt->getBody()->accept(this);
    --m_loopDepth;
    return {};
}

std::any SemaAnalyzer::visit(DoStmt* stmt) {
    if (!stmt) return {};
    ++m_loopDepth;
    if (stmt->getBody()) stmt->getBody()->accept(this);
    --m_loopDepth;
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isScalarType()) {
            m_diag->warning("do-while condition is not a scalar type in function '" + m_currentFuncName + "'");
        }
    }
    return {};
}

std::any SemaAnalyzer::visit(ForStmt* stmt) {
    if (!stmt) return {};
    if (stmt->getInit()) stmt->getInit()->accept(this);
    if (stmt->getCond()) {
        QualType condType = std::any_cast<QualType>(stmt->getCond()->accept(this));
        if (!condType.isNull() && !condType->isScalarType()) {
            m_diag->warning("for condition is not a scalar type in function '" + m_currentFuncName + "'");
        }
    }
    if (stmt->getUpdate()) stmt->getUpdate()->accept(this);
    ++m_loopDepth;
    if (stmt->getBody()) stmt->getBody()->accept(this);
    --m_loopDepth;
    return {};
}

std::any SemaAnalyzer::visit(GotoStmt* stmt) {
    if (!stmt) return {};
    // Goto 目标标签 -- 由于当前 AST 不记录标签名，暂不检查
    return {};
}

std::any SemaAnalyzer::visit(ContinueStmt* stmt) {
    if (!stmt) return {};
    if (m_loopDepth <= 0) {
        m_diag->error("'continue' outside loop in function '" + m_currentFuncName + "'");
    }
    return {};
}

std::any SemaAnalyzer::visit(BreakStmt* stmt) {
    if (!stmt) return {};
    if (m_loopDepth <= 0 && m_switchDepth <= 0) {
        m_diag->error("'break' outside loop or switch in function '" + m_currentFuncName + "'");
    }
    return {};
}

std::any SemaAnalyzer::visit(ReturnStmt* stmt) {
    if (!stmt) return {};
    Expr* retExpr = stmt->getRetExpr();

    if (retExpr) {
        // 有返回值表达式
        QualType exprType = std::any_cast<QualType>(retExpr->accept(this));

        if (!m_currentRetType.isNull()) {
            if (m_currentRetType->isVoidType()) {
                // void 函数不应有返回值
                m_diag->error("void function '" + m_currentFuncName + "' should not return a value");
            } else if (!isTypeCompatible(exprType, m_currentRetType)) {
                // 返回值类型不兼容
                m_diag->error("return type '" + exprType->getName()
                    + "' does not match function return type '" + m_currentRetType->getName()
                    + "' in function '" + m_currentFuncName + "'");
            } else if (exprType != m_currentRetType) {
                // 需要隐式转换
                if (m_currentRetType->isArithmeticType()) {
                    stmt->setRetExpr(insertImplicitConversion(retExpr, m_currentRetType, CastExpr::CK_AssignmentConversion));
                }
            }
        }
    } else {
        // 无返回值表达式
        if (!m_currentRetType.isNull() && !m_currentRetType->isVoidType()) {
            m_diag->error("non-void function '" + m_currentFuncName + "' should return a value");
        }
    }
    return {};
}

// ============================================================
// 表达式节点的 visit 方法
// ============================================================

std::any SemaAnalyzer::visit(IntegerLiteral* expr) {
    if (!expr) return QualType();
    // Parser 已设置类型 (long long)，直接返回
    return expr->getType();
}

std::any SemaAnalyzer::visit(FloatingLiteral* expr) {
    if (!expr) return QualType();
    // Parser 已设置类型 (double)，直接返回
    return expr->getType();
}

std::any SemaAnalyzer::visit(CharacterLiteral* expr) {
    if (!expr) return QualType();
    // Parser 已设置类型 (char)，直接返回
    return expr->getType();
}

std::any SemaAnalyzer::visit(StringLiteral* expr) {
    if (!expr) return QualType();
    // 字符串字面量类型为 char*，但 parser 可能未设置类型
    if (expr->getType().isNull()) {
        auto charType = m_ctx->getBuiltTypeByTS(TypeSpecifier::CHAR);
        expr->setType(QualType(Arena::make<PointerType>(QualType(charType))));
    }
    return expr->getType();
}

std::any SemaAnalyzer::visit(DeclRefExpr* expr) {
    if (!expr) return QualType();
    QualType qt = expr->getType();
    // 如果表达式类型为空，尝试从符号表获取
    if (qt.isNull()) {
        auto sym = m_ctx->LookupNormal(expr->getDecl() ? expr->getDecl()->getName() : "");
        if (sym) {
            qt = sym->getType();
            expr->setType(qt);
        }
    }
    // 数组到指针衰减: 如果引用的是数组，返回指针类型
    if (!qt.isNull() && qt->isArrayType()) {
        auto arrType = qt.as<ArrayType>();
        if (arrType) {
            QualType elemType = arrType->getQualType();
            auto ptrType = QualType(Arena::make<PointerType>(elemType));
            // 这里不修改原表达式类型（保持为数组类型），而是返回指针类型
            // 实际衰减在值上下文由父节点处理
            return ptrType;
        }
    }
    return qt;
}

std::any SemaAnalyzer::visit(ParenExpr* expr) {
    if (!expr) return QualType();
    // 括号表达式类型等于内部表达式类型
    if (expr->getSubExpr()) {
        QualType inner = std::any_cast<QualType>(expr->getSubExpr()->accept(this));
        expr->setType(inner);
        return inner;
    }
    return expr->getType();
}

std::any SemaAnalyzer::visit(BinaryOpExpr* expr) {
    if (!expr) return QualType();

    Expr* lhs = expr->getLExpr();
    Expr* rhs = expr->getRExpr();

    if (!lhs || !rhs) {
        return QualType();
    }

    // 先访问左右操作数进行类型推断
    QualType lhsType = std::any_cast<QualType>(lhs->accept(this));
    QualType rhsType = std::any_cast<QualType>(rhs->accept(this));

    if (lhsType.isNull() || rhsType.isNull()) {
        return QualType();
    }

    BinaryOpExpr::OpCode op = expr->getOpCode();

    // 赋值操作
    if (op >= BinaryOpExpr::Assign_ && op <= BinaryOpExpr::BitWise_OR_Assign_) {
        // 检查左操作数是否为可修改左值
        if (!isModifiableLValue(lhs)) {
            m_diag->error("assignment target is not a modifiable lvalue in function '" + m_currentFuncName + "'");
        }
        // 赋值转换: rhs 类型转为 lhs 类型
        if (lhsType->isArithmeticType() && rhsType->isArithmeticType()) {
            if (lhsType != rhsType) {
                expr->setRExpr(insertImplicitConversion(rhs, lhsType, CastExpr::CK_AssignmentConversion));
            }
        }
        // 复合赋值操作 (+=, -= 等)
        QualType resultType = lhsType;
        expr->setType(resultType);
        return resultType;
    }

    // 逻辑操作 (&&, ||)
    if (op == BinaryOpExpr::Logical_AND_ || op == BinaryOpExpr::Logical_OR_) {
        // 操作数应为标量类型，结果类型为 int
        if (!lhsType->isScalarType()) {
            m_diag->warning("left operand of logical operator is not scalar in function '" + m_currentFuncName + "'");
        }
        if (!rhsType->isScalarType()) {
            m_diag->warning("right operand of logical operator is not scalar in function '" + m_currentFuncName + "'");
        }
        QualType intType = QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::INT));
        expr->setType(intType);
        return intType;
    }

    // 逗号表达式
    if (op == BinaryOpExpr::Comma) {
        // 逗号表达式的结果类型是右操作数的类型
        expr->setType(rhsType);
        return rhsType;
    }

    // 关系操作 (<, >, <=, >=, ==, !=)
    if (op == BinaryOpExpr::Less_ || op == BinaryOpExpr::Greater_ ||
        op == BinaryOpExpr::Less_Equal_ || op == BinaryOpExpr::Greater_Equal_ ||
        op == BinaryOpExpr::Equality_ || op == BinaryOpExpr::Inequality_) {
        // 执行常用算术转换
        applyUsualArithmeticConversions(lhs, rhs);
        // 比较操作需要实际检查
        if (!lhsType->isScalarType() && !rhsType->isScalarType() &&
            !lhsType->isPointerType() && !rhsType->isPointerType()) {
            m_diag->warning("comparison between non-scalar types in function '" + m_currentFuncName + "'");
        }
        // 结果类型为 int
        QualType intType = QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::INT));
        expr->setType(intType);
        return intType;
    }

    // 算术操作 (+, -, *, /, %)
    if (op == BinaryOpExpr::Addition_ || op == BinaryOpExpr::Subtraction_ ||
        op == BinaryOpExpr::Multiplication_ || op == BinaryOpExpr::Division_ ||
        op == BinaryOpExpr::Modulus_) {

        // 指针运算: ptr + int, int + ptr, ptr - int
        if (lhsType->isPointerType() && rhsType->isIntegerType()) {
            expr->setType(lhsType);
            return lhsType;
        }
        if (rhsType->isPointerType() && lhsType->isIntegerType() && op == BinaryOpExpr::Addition_) {
            expr->setType(rhsType);
            return rhsType;
        }
        if (lhsType->isPointerType() && rhsType->isPointerType() && op == BinaryOpExpr::Subtraction_) {
            // ptrdiff_t (int)
            QualType intType = QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::INT));
            expr->setType(intType);
            return intType;
        }

        // 取模要求整数类型
        if (op == BinaryOpExpr::Modulus_) {
            if (!lhsType->isIntegerType() || !rhsType->isIntegerType()) {
                m_diag->error("operands of '%' must have integer types in function '" + m_currentFuncName + "'");
            }
        }

        // 常用算术转换
        applyUsualArithmeticConversions(lhs, rhs);
        QualType commonType = getCommonRealType(lhsType, rhsType);
        expr->setType(commonType);
        return commonType;
    }

    // 移位操作 (<<, >>)
    if (op == BinaryOpExpr::LShift_ || op == BinaryOpExpr::RShift_) {
        // 左操作数执行整数提升，结果类型为提升后的左操作数类型
        lhs = applyIntegerPromotions(lhs);
        QualType promoted = getIntegerPromotionType(lhsType);
        expr->setType(promoted);
        return promoted;
    }

    // 位运算 (&, |, ^)
    if (op == BinaryOpExpr::BitWise_AND_ || op == BinaryOpExpr::BitWise_OR_ ||
        op == BinaryOpExpr::BitWise_XOR_) {
        // 操作数应为整数类型
        if (!lhsType->isIntegerType()) {
            m_diag->warning("left operand of bitwise operator is not an integer in function '" + m_currentFuncName + "'");
        }
        if (!rhsType->isIntegerType()) {
            m_diag->warning("right operand of bitwise operator is not an integer in function '" + m_currentFuncName + "'");
        }
        // 执行整数提升
        applyUsualArithmeticConversions(lhs, rhs);
        QualType commonType = getCommonRealType(lhsType, rhsType);
        expr->setType(commonType);
        return commonType;
    }

    return lhsType;
}

std::any SemaAnalyzer::visit(ConditionalExpr* expr) {
    if (!expr) return QualType();
    // 条件 ? 真 : 假
    if (expr->getCond()) {
        QualType condType = std::any_cast<QualType>(expr->getCond()->accept(this));
        if (!condType.isNull() && !condType->isScalarType()) {
            m_diag->warning("condition of conditional expression is not scalar in function '" + m_currentFuncName + "'");
        }
    }
    QualType thenType, elseType;
    if (expr->getThen()) {
        thenType = std::any_cast<QualType>(expr->getThen()->accept(this));
    }
    if (expr->getElse()) {
        elseType = std::any_cast<QualType>(expr->getElse()->accept(this));
    }
    // 结果类型: then 和 else 的公共类型
    if (!thenType.isNull() && !elseType.isNull()) {
        QualType commonType = getCommonRealType(thenType, elseType);
        expr->setType(commonType);
        return commonType;
    }
    if (!thenType.isNull()) {
        expr->setType(thenType);
        return thenType;
    }
    return elseType;
}

std::any SemaAnalyzer::visit(CompoundLiteralExpr* expr) {
    if (!expr) return QualType();
    QualType qt = expr->getType();
    if (expr->getInitExpr()) {
        qt = std::any_cast<QualType>(expr->getInitExpr()->accept(this));
    }
    return qt;
}

std::any SemaAnalyzer::visit(CastExpr* expr) {
    if (!expr) return QualType();
    // 显式类型转换
    Expr* subExpr = expr->getCastExpr();
    if (subExpr) {
        subExpr->accept(this);
    }
    // 返回转换的目标类型
    return expr->getType();
}

std::any SemaAnalyzer::visit(ArraySubscriptExpr* expr) {
    if (!expr) return QualType();
    Expr* base = expr->getBaseExpr();
    Expr* index = expr->getIndexExpr();

    if (base && index) {
        QualType baseType = std::any_cast<QualType>(base->accept(this));
        QualType indexType = std::any_cast<QualType>(index->accept(this));

        // 检查 index 是否为整数类型
        if (!indexType.isNull() && !indexType->isIntegerType()) {
            m_diag->error("array subscript must be an integer type in function '" + m_currentFuncName + "'");
        }

        // 结果类型: base 是 array[T] 或 T* 时结果为 T
        if (!baseType.isNull()) {
            if (baseType->isPointerType()) {
                auto ptrType = baseType.as<PointerType>();
                if (ptrType) {
                    QualType elemType = ptrType->getPointeeType();
                    expr->setType(elemType);
                    return elemType;
                }
            } else if (baseType->isArrayType()) {
                auto arrType = baseType.as<ArrayType>();
                if (arrType) {
                    QualType elemType = arrType->getQualType();
                    expr->setType(elemType);
                    return elemType;
                }
            } else {
                m_diag->error("subscripted value is not an array or pointer in function '" + m_currentFuncName + "'");
            }
        }
    }
    return expr->getType();
}

std::any SemaAnalyzer::visit(CallExpr* expr) {
    if (!expr) return QualType();

    // 检查被调用者
    Expr* callee = expr->getCallee();
    if (!callee) {
        m_diag->error("function call with no callee in function '" + m_currentFuncName + "'");
        return QualType();
    }

    QualType calleeType = std::any_cast<QualType>(callee->accept(this));

    if (calleeType.isNull()) {
        return QualType();
    }

    // 检查被调用者是否为函数指针类型
    QualType funcType;
    if (calleeType->isFunctionType()) {
        funcType = calleeType;
    } else if (calleeType->isPointerType()) {
        auto ptrType = calleeType.as<PointerType>();
        if (ptrType && ptrType->getPointeeType()->isFunctionType()) {
            funcType = ptrType->getPointeeType();
        }
    }

    if (funcType.isNull()) {
        m_diag->error("called object is not a function or function pointer in function '" + m_currentFuncName + "'");
        return QualType();
    }

    auto ft = funcType.as<FunctionType>();
    if (!ft) {
        return QualType();
    }

    // 检查参数类型匹配
    auto paramTypes = ft->getParams();
    auto args = expr->getParams();

    if (args.size() != paramTypes.size()) {
        m_diag->error("function call with " + std::to_string(args.size())
            + " arguments but function expects " + std::to_string(paramTypes.size())
            + " in function '" + m_currentFuncName + "'");
    } else {
        // 逐个检查参数类型
        for (size_t i = 0; i < args.size() && i < paramTypes.size(); ++i) {
            QualType argType = std::any_cast<QualType>(args[i]->accept(this));
            QualType paramType = paramTypes[i];
            if (!argType.isNull() && !paramType.isNull()) {
                if (!isTypeCompatible(argType, paramType)) {
                    m_diag->warning("argument " + std::to_string(i + 1)
                        + " type '" + argType->getName()
                        + "' does not match parameter type '" + paramType->getName()
                        + "' in function '" + m_currentFuncName + "'");
                } else if (argType != paramType && paramType->isArithmeticType()) {
                    // 插入函数参数隐式转换
                    auto converted = insertImplicitConversion(args[i], paramType, CastExpr::CK_FunctionArgPromotion);
                    // 由于无法直接修改 CallExpr 的参数 vector，我们通过 accept 返回转换信息
                    // 这里仅做检查
                }
            }
        }
    }

    // 结果类型为函数返回类型
    QualType retType = ft->getRetType();
    expr->setType(retType);
    return retType;
}

std::any SemaAnalyzer::visit(MemberExpr* expr) {
    if (!expr) return QualType();
    // 成员表达式 - 访问父结构体并获取成员类型
    if (expr->getParent()) {
        expr->getParent()->accept(this);
    }
    if (expr->getMember()) {
        expr->getMember()->accept(this);
    }
    // 返回已设置的类型
    return expr->getType();
}

std::any SemaAnalyzer::visit(UnaryOpExpr* expr) {
    if (!expr) return QualType();
    Expr* subExpr = expr->getSubExpr();
    if (!subExpr) return QualType();

    QualType subType = std::any_cast<QualType>(subExpr->accept(this));
    if (subType.isNull()) return QualType();

    UnaryOpExpr::OpCode op = expr->getOpCode();

    switch (op) {
        case UnaryOpExpr::Post_Increment_:
        case UnaryOpExpr::Post_Decrement_:
        case UnaryOpExpr::Pre_Increment_:
        case UnaryOpExpr::Pre_Decrement_: {
            // ++/-- 需要可修改左值
            if (!isModifiableLValue(subExpr)) {
                m_diag->error("increment/decrement operand must be a modifiable lvalue in function '"
                    + m_currentFuncName + "'");
            }
            if (!subType->isArithmeticType() && !subType->isPointerType()) {
                m_diag->error("increment/decrement requires arithmetic or pointer type in function '"
                    + m_currentFuncName + "'");
            }
            expr->setType(subType);
            return subType;
        }

        case UnaryOpExpr::Multiplication_: {
            // 解引用: *ptr
            if (!subType->isPointerType()) {
                m_diag->error("indirection requires pointer operand in function '" + m_currentFuncName + "'");
                return QualType();
            }
            auto ptrType = subType.as<PointerType>();
            if (ptrType) {
                QualType pointeeType = ptrType->getPointeeType();
                expr->setType(pointeeType);
                return pointeeType;
            }
            return QualType();
        }

        case UnaryOpExpr::BitWise_AND_: {
            // 取地址: &lvalue
            if (!isLValue(subExpr)) {
                m_diag->error("address-of operator requires an lvalue in function '" + m_currentFuncName + "'");
            }
            QualType ptrType = QualType(Arena::make<PointerType>(subType));
            expr->setType(ptrType);
            return ptrType;
        }

        case UnaryOpExpr::Addition_: {
            // 一元正号: +expr
            QualType promoted = getIntegerPromotionType(subType);
            expr->setType(promoted);
            return promoted;
        }

        case UnaryOpExpr::Subtraction_: {
            // 一元负号: -expr
            if (!subType->isArithmeticType()) {
                m_diag->error("unary minus requires arithmetic type in function '" + m_currentFuncName + "'");
            }
            QualType promoted = getIntegerPromotionType(subType);
            expr->setType(promoted);
            return promoted;
        }

        case UnaryOpExpr::BitWise_NOT_: {
            // 按位取反: ~expr
            if (!subType->isIntegerType()) {
                m_diag->error("bitwise complement requires integer type in function '" + m_currentFuncName + "'");
            }
            QualType promoted = getIntegerPromotionType(subType);
            expr->setType(promoted);
            return promoted;
        }

        case UnaryOpExpr::Logical_NOT_: {
            // 逻辑非: !expr
            if (!subType->isScalarType()) {
                m_diag->warning("logical not operand is not scalar in function '" + m_currentFuncName + "'");
            }
            QualType intType = QualType(m_ctx->getBuiltTypeByTS(TypeSpecifier::INT));
            expr->setType(intType);
            return intType;
        }

        default:
            expr->setType(subType);
            return subType;
    }
}
