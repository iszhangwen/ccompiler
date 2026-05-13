#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace ccompiler {

// 诊断信息中的源代码位置
struct DiagLocation {
    std::string filename;
    int line;
    int column;

    DiagLocation() : filename(""), line(0), column(0) {}
    DiagLocation(const std::string& f, int l, int c)
        : filename(f), line(l), column(c) {}
};

class DiagnosticEngine {
public:
    enum Level { Note, Warning, Error };

    DiagnosticEngine(bool immediateOutput = false);

    // 核心报告接口
    void report(Level level, const DiagLocation& loc, const std::string& msg);
    void report(Level level, const std::string& msg);

    // 便捷方法
    void error(const DiagLocation& loc, const std::string& msg);
    void error(const std::string& msg);
    void warning(const DiagLocation& loc, const std::string& msg);
    void warning(const std::string& msg);
    void note(const DiagLocation& loc, const std::string& msg);
    void note(const std::string& msg);

    // 查询
    int getErrorCount() const { return m_errorCount; }
    int getWarningCount() const { return m_warningCount; }
    bool hasErrors() const { return m_errorCount > 0; }

    // 输出所有诊断
    void printAll(std::ostream& os = std::cerr);

    // 重置
    void clear();

private:
    struct Diagnostic {
        Level level;
        DiagLocation loc;
        bool hasLocation;
        std::string message;
    };

    std::vector<Diagnostic> m_diagnostics;
    int m_errorCount;
    int m_warningCount;
    bool m_immediateOutput;

    std::string formatMessage(const Diagnostic& d);
    std::string levelToString(Level level);
    const char* levelColor(Level level);
    const char* colorReset();
};

} // namespace ccompiler
