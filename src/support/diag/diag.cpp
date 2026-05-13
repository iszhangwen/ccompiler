#include "diag.h"

using namespace ccompiler;

DiagnosticEngine::DiagnosticEngine(bool immediateOutput)
    : m_errorCount(0), m_warningCount(0), m_immediateOutput(immediateOutput) {}

void DiagnosticEngine::report(Level level, const DiagLocation& loc, const std::string& msg) {
    Diagnostic d;
    d.level = level;
    d.loc = loc;
    d.hasLocation = true;
    d.message = msg;

    if (level == Error) ++m_errorCount;
    else if (level == Warning) ++m_warningCount;

    m_diagnostics.push_back(d);

    if (m_immediateOutput) {
        std::cerr << formatMessage(d) << std::endl;
    }
}

void DiagnosticEngine::report(Level level, const std::string& msg) {
    Diagnostic d;
    d.level = level;
    d.hasLocation = false;
    d.message = msg;

    if (level == Error) ++m_errorCount;
    else if (level == Warning) ++m_warningCount;

    m_diagnostics.push_back(d);

    if (m_immediateOutput) {
        std::cerr << formatMessage(d) << std::endl;
    }
}

void DiagnosticEngine::error(const DiagLocation& loc, const std::string& msg) {
    report(Error, loc, msg);
}

void DiagnosticEngine::error(const std::string& msg) {
    report(Error, msg);
}

void DiagnosticEngine::warning(const DiagLocation& loc, const std::string& msg) {
    report(Warning, loc, msg);
}

void DiagnosticEngine::warning(const std::string& msg) {
    report(Warning, msg);
}

void DiagnosticEngine::note(const DiagLocation& loc, const std::string& msg) {
    report(Note, loc, msg);
}

void DiagnosticEngine::note(const std::string& msg) {
    report(Note, msg);
}

const char* DiagnosticEngine::levelColor(Level level) {
    switch (level) {
        case Error:   return "\033[31m"; // 红色
        case Warning: return "\033[33m"; // 黄色
        case Note:    return "\033[36m"; // 青色
        default:      return "";
    }
}

const char* DiagnosticEngine::colorReset() {
    return "\033[0m";
}

std::string DiagnosticEngine::levelToString(Level level) {
    switch (level) {
        case Error:   return "error";
        case Warning: return "warning";
        case Note:    return "note";
        default:      return "";
    }
}

std::string DiagnosticEngine::formatMessage(const Diagnostic& d) {
    std::stringstream ss;
    if (d.hasLocation) {
        ss << d.loc.filename
           << ":" << d.loc.line
           << ":" << d.loc.column
           << ": ";
    }
    ss << levelColor(d.level)
       << levelToString(d.level) << ": "
       << colorReset()
       << d.message;
    return ss.str();
}

void DiagnosticEngine::printAll(std::ostream& os) {
    for (const auto& d : m_diagnostics) {
        os << formatMessage(d) << std::endl;
    }
    // 汇总信息
    if (m_errorCount > 0 || m_warningCount > 0) {
        std::stringstream summary;
        if (m_errorCount > 0) {
            summary << levelColor(Error)
                    << "compilation terminated: "
                    << m_errorCount << " error(s)"
                    << colorReset();
        }
        if (m_warningCount > 0) {
            if (m_errorCount > 0) summary << ", ";
            summary << levelColor(Warning)
                    << m_warningCount << " warning(s)"
                    << colorReset();
        }
        os << summary.str() << std::endl;
    }
}

void DiagnosticEngine::clear() {
    m_diagnostics.clear();
    m_errorCount = 0;
    m_warningCount = 0;
}
