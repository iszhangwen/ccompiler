#include "source.h"
#include <fstream>
#include <iostream>

Source::Source(const std::string& fileName)
{
    segment.clear();
    loc_.first.line = 0;
    loc_.first.column = 0;
    loc_.first.filename = fileName;
    loc_.second.line = 0;
    loc_.second.column = 0;
    loc_.second.filename = fileName;
    load(fileName);
}

Source::Source(const Source& other)
{
    segment = other.segment;
    loc_ = other.loc_;
    src_ = other.src_;
}

Source& Source::operator=(const Source& other)
{
    segment = other.segment;
    loc_ = other.loc_;
    src_ = other.src_;
}

Source::Source(Source&& other)
{
    segment = other.segment;
    loc_ = other.loc_;
    src_ = other.src_;
}

Source& Source::operator=(Source&& other)
{
    segment = other.segment;
    loc_ = other.loc_;
    src_ = other.src_;
}

SourceLocation Source::loc() const
{
    return loc_.first;
}
// 标记开始读取位置
void Source::mark()
{
    segment.clear();
    segment.push_back(curch());
    loc_.first = loc_.second;
}
// 读取当前字符
char Source::curch() const
{
    int row = loc_.second.line;
    int col = loc_.second.column;
    return src_[row]->at(col);
}
// 读取下一个字符
char Source::nextch()
{
    char ch = curch();
    switch (ch)
    {
    case '\n':
        loc_.second.line++;
        loc_.second.column = 0;
        break;
    case EOF:
        break;
    default:
        loc_.second.column++;
        break;
    }
    ch = curch();
    segment.push_back(ch);
    return ch;
}
// 预读前n个字符， 不移动指针
char Source::peek(size_t n)
{
    int row = loc_.second.line;
    int col = loc_.second.column;

    while (n > 0)
    {
        char ch = src_[row]->at(col);
        if (ch == '\n')
        {
            row++;
            col = 0;
        }
        else if (ch == EOF)
        {
            return EOF;
        }
        else
        {
            col++;
        }
        n--;
    }
    return src_[row]->at(col);
}
// 匹配下一个字符，若相同则移动指针并返回true
bool Source::match(char ch)
{
    if (peek() == ch)
    {
        nextch();
        return true;
    }
    return false;
}
// 返回文件是否已读取完毕
bool Source::is_end() const
{
    return curch() == EOF;
}

std::string Source::segline() const
{
    int row = loc_.first.line;
    return *src_[row];
}

// 返回当前标记的行字符串, 主要是为了输出错误信息
std::string Source::segline(SourceLocation loc) const
{
    int row = loc.line;
    return *src_[row];
}
// 返回对当前标记的字符串，主要是为了Token分词，会重置mark标记, 最后一个字符属于Tokend
std::string Source::seg() const
{
    std::string res;
    for (int i = 0; i < segment.size() - 1; i++)
    {
        res.push_back(segment.at(i));
    }
    return res;
}

bool Source::load(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        throw ("the file isn't exist!");
        return false;
    }

    char ch;
    auto line = std::make_shared<std::string>();
    while (file.get(ch))
    {
        line->push_back(ch);
        if (ch == '\n')
        {
            src_.push_back(line);
            line = std::make_shared<std::string>();
        }
    }

    if (file.eof())
    {
        if (!line->empty())
        {
            // 最后一行可能没有换行符，手动添加
            if (line->back() != '\n')
            {
                line->push_back('\n');
            }
            src_.push_back(line);
        }
        src_.push_back(std::make_shared<std::string>(1, EOF));
    }
    return true;
}

