#pragma once
#include <string>
#include <exception>

class CCError : public std::exception
{
public:
    const char* what() const noexcept override 
    {
        return err_.c_str();
    } 
    
    CCError(const std::string& val):err_(val){}
    CCError(CCError&& obj);
    CCError(const CCError& obj);
    CCError& operator=(const CCError& obj);
    CCError& operator=(CCError&& obj);
private:
    std::string err_;
};
