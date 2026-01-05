#include "ccerror.h"

CCError::CCError(CCError&& obj)
{
    err_ = obj.err_;
}

CCError::CCError(const CCError& obj)
{
    err_ = obj.err_;
}

CCError& CCError::operator=(const CCError& obj)
{
    err_ = obj.err_;
    return *this;
}

CCError& CCError::operator=(CCError&& obj)
{
    err_ = obj.err_;
    return *this;
}