#pragma once

#include <string>
#include <list>
#include <unordered_map>

class ModuleFunction;

class MachineModule
{
public:
    MachineModule(){}
    ~MachineModule() = default;

private:
    // 模块名
    std::string m_name;
    // 函数
    std::list<ModuleFunction*> m_functions;
    // 函数名映射
    std::unordered_map<std::string, ModuleFunction*> m_str2Function;
};