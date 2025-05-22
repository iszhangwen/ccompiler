#pragma once
#include <string>
#include <vector>

// 源码字符位置
struct SourceLocation {
    SourceLocation(){}

    explicit SourceLocation(std::string filename)
    : filename(filename), line(0), column(0){}

    SourceLocation(const SourceLocation& loc)
    : filename(loc.filename), line(loc.line), column(loc.column) {}

    SourceLocation& operator=(const SourceLocation& loc) {
        if (this != &loc) {
            filename = loc.filename;
            line = loc.line;
            column = loc.column;
        }
        return *this;
    }
    std::string filename;
    int line;
    int column;
};

/*
该类将一个文件的源码读入到内存中，并记录每个字符的位置，按照行存储数据。
*/
class Source 
{
public:
    explicit Source(const std::string& fileName);
    // 获取当前的读取的开始位置
    SourceLocation loc() const;
    // 标记开始读取位置
    void mark();
    // 读取当前字符
    char curch() const;
    // 读取下一个字符
    char nextch();
    // 预读前n个字符， 不移动指针
    char peek(size_t n = 1);
    // 匹配下一个字符，若相同则移动指针到下个元素
    bool match(char ch);
    // 返回文件是否已读取完毕
    bool is_end() const;
    // 返回当前标记的行字符串, 主要是为了输出错误信息
    std::string segline() const;
    // 返回对当前标记的字符串，主要是为了Token分词，会重置mark标记
    std::string seg() const;

private:
    // 缓存当前的Token词
    std::string segment;
    // 当前mark标记的位置，和读取到的位置
    std::pair<SourceLocation, SourceLocation> loc_; 
    std::vector<std::string*> src_; // 源码
    // 读取当前文件
    bool load(const std::string& filePath);
};
