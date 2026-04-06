#pragma once

#include <string>
#include <list>
#include <unordered_map>

#include "machinefunction.h"

class MachineModule
{
public:
    MachineModule() : m_curFunc(nullptr) {}
    ~MachineModule() = default;

    // 模块名
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }

    // 函数操作
    void addFunction(MachineFunction* func) {
        m_functions.push_back(func);
        m_name2Func[func->getName()] = func;
    }

    std::list<MachineFunction*> getFunctions() { return m_functions; }
    MachineFunction* getFunction(const std::string& name) {
        auto it = m_name2Func.find(name);
        return (it != m_name2Func.end()) ? it->second : nullptr;
    }

    // 当前编译的函数
    void setCurFunction(MachineFunction* func) { m_curFunc = func; }
    MachineFunction* getCurFunction() { return m_curFunc; }

private:
    std::string m_name;
    std::list<MachineFunction*> m_functions;
    std::unordered_map<std::string, MachineFunction*> m_name2Func;
    MachineFunction* m_curFunc;
};
