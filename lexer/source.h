#pragma once
#include <string>
#include <vector>
#include <fstream>

// 源码字符位置
struct SourceLocation {
    explicit SourceLocation(std::string filename):
    filename(filename), line(1), coloum(1){}

    SourceLocation(const SourceLocation& loc) {
        filename = loc.filename;
        line = loc.line;
        coloum = loc.coloum;
    }

    std::string filename;
    int line;
    int coloum;
};

// 源码输入流管理
class Source 
{
private:
    std::ifstream file;
    std::vector<char> buffer;
    SourceLocation location; // location 记录当前的文档字母位置

    bool is_eof;
    size_t curPos;
    size_t endPos;
    size_t streamPos;
    /*
        fill buffer
    */
    void fillBuffer(size_t offset);
public:
    explicit Source(std::string filename, size_t buffserSize);
    ~Source();

    char read();
    char peek(size_t n = 0);
    bool eof() const {return is_eof && (curPos >= endPos);}
    SourceLocation pos() const {return location;}
};

// 源码输入缓冲
class SourceBuffer {
private:
    Source buffer;
    SourceLocation location; // location 记录segment的第一个字母位置
    std::string segment;
public:
    explicit SourceBuffer(std::string filename, size_t buffserSize = 1024):
    buffer(filename, buffserSize), location(filename) {}
    // get alphabet
    void mark();

    char curch();
    char nextch();
    char peek(size_t n = 0);
    bool match(char);

    std::string ObtainSegment() const;
    // obtaion curent location. and token location Token record.
    SourceLocation ObtainLocation() const;
};
