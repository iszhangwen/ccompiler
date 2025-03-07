# 翻译方案设计：语法制导翻译

## 语法分析步骤
```
    (1) 将C89文法转化为LL(1)文法
    (2) 使用L-翻译模式
   （3）添加语义动作(sem action)
    (4) 定义抽象语法树节点
   （5）递归下降分析LL(1)
```

## C89基础语法
### External definitions
translation-unit:
    external-declaration 
    | translation-unit external-declaration

external-declaration:
    function-definition
    | declaration


funtion-definnition:
    declaration-specifiers declarator declaration-list[opt] compound-statement

declaration-list:
    declaration
    | declaration-list declaration

### Expressions


## 转换为LL(1)文法
### 消除二义性
### 消除左递归
### 提取左公因式