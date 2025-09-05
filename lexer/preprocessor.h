#pragma once
#include <token.h>
#include <scanner.h>

class Preprocessor {
public:
    // 预处理器的指令类型
    enum class DirectiveType {
        Include,         // 包含文件指令
        Define,          // 宏定义指令
        Undefine,        // 宏取消定义指令
        If,              // 条件编译指令
        Ifdef,           // 如果宏已定义指令
        Ifndef,          // 如果宏未定义指令
        Else,            // 否则指令
        Endif,           // 结束条件编译指令
        Error,           // 错误指令
        Pragma           // 编译器特定的指令
    };

    // 预处理器的指令
    struct Directive {
        DirectiveType type;
        std::string name; // 指令名称
        std::string value; // 指令值（如果有的话）
    };

    // 预处理器的宏定义
    struct MacroDefinition {
        std::string name; // 宏名称
        std::string replacement; // 替换文本
        bool isFunctionLike; // 是否为函数式宏
    };

    // 预处理器的条件状态
    struct ConditionState {
        bool isActive; // 条件是否激活
        std::vector<Directive> directives; // 条件下的指令
    };

    // 预处理器的文件管理
    struct FileState {
        std::string fileName; // 文件名
        int lineNumber; // 当前行号
        std::vector<MacroDefinition> macros; // 文件中的宏定义
    };

    // 预处理器的状态机
    struct PreprocessorState {
        ConditionState conditionState; // 条件状态
        FileState fileState; // 文件状态
        std::vector<Directive> directives; // 已处理的指令
    };
    
    // 预处理器的宏管理器
    struct MacroManager {
        std::unordered_map<std::string, MacroDefinition> macros; // 宏定义映射
        void defineMacro(const std::string& name, const std::string& replacement, bool isFunctionLike);
        void undefineMacro(const std::string& name);
        bool isMacroDefined(const std::string& name) const;
        MacroDefinition getMacro(const std::string& name) const;
    };
    // 预处理器的文件管理器
    struct FileManager {
        std::unordered_map<std::string, FileState> files; // 文件状态映射
        void addFile(const std::string& fileName);
        void removeFile(const std::string& fileName);
        FileState getFileState(const std::string& fileName) const;
    };
    // 预处理器的条件状态管理器
    struct ConditionStateManager {
        std::vector<ConditionState> states; // 条件状态栈
        void pushState(const ConditionState& state);
        void popState();
        ConditionState currentState() const;
    };
    
    // 预处理器的错误处理
    struct Error {
        std::string message; // 错误信息
        SourceLocation location; // 错误位置
    };  
    // 预处理器的错误管理
    struct ErrorManager {
        std::vector<Error> errors; // 错误列表
        void addError(const std::string& message, const SourceLocation& location);
        void clearErrors();
        const std::vector<Error>& getErrors() const;
    };
    // 预处理器的状态
    enum State {
        Normal,          // 正常状态
        InMacro,         // 在宏定义中
        InIfDef,         // 在条件编译中
        InInclude,       // 在包含文件中
        InComment,       // 在注释中
        InStringLiteral, // 在字符串字面量中
        InCharacterLiteral // 在字符字面量中
    };

private:
    Source *buf_;
    State currentState_{State::Normal};
    MacroManager macroManager_;
    FileManager fileManager_;
    ConditionStateManager conditionStateManager_;
    // 错误报告处理
    void PreprocessorError(SourceLocation loc, const std::string& val);
    // 处理宏定义
    void handleMacroDefinition();
    // 处理取消宏定义
    void handleMacroUnDefinition();
    // 处理条件编译指令
    void handleConditionDirective();
    // 处理包含文件指令
    void handleIncludeDirective();
    // 处理注释
    void handleComment();
    // 处理字符串和字符字面量
    void handleStringAndCharacterLiterals();
    // 展开宏定义
    void handleExpandMacro();
    //
    void process(TokenSequence& is, TokenSequence& os)
public:
    Preprocessor(){}
    TokenSequence preprocess(Source* buf);
};