#include "source.h"

Source::Source(std::string filename, size_t buffserSize = 1024):
location(filename), file(filename, std::ios::binary), is_eof(false), curPos(0),endPos(0), streamPos(0)
{
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + filename);
    }
    buffer.resize(buffserSize);
    fillBuffer(0);
}

Source::~Source()
{
    if (file.is_open()) {
        file.close();
    }
}

void Source::fillBuffer(size_t offset)
{
    size_t requiredPos = curPos + offset;
    if (requiredPos < endPos) {
        return;
    }
    // 计算需要读多少个字节
    size_t need = requiredPos - endPos + 1;
    // 计算当前可用的空间，若空间不足则移动数据到首，若再不足则扩展空间。
    size_t availableSpace = buffer.size() - endPos;
    if (availableSpace < need) {
        size_t remainPos = endPos - curPos;
        std::copy(buffer.begin() + curPos, buffer.begin() + endPos, buffer.begin());
        
        streamPos += curPos;
        curPos = 0;
        endPos = remainPos;
        availableSpace = buffer.size() - endPos;
        if (availableSpace < need) {
            buffer.resize(buffer.size() + (need - availableSpace));
            availableSpace = buffer.size() - endPos;
        }
    }
    // 从文件读取数据到缓冲区
    file.seekg(streamPos + endPos);
    file.read(buffer.data() + endPos, availableSpace);
    size_t readBytes = file.gcount();
    endPos += readBytes;

    if (readBytes == 0) {
        is_eof = true;
    }
}

char Source::read()
{
    char ch = peek();
    // read时更新locaiton位置
    if (ch == '\n') {
        location.column = 1;
        location.line ++;
    }  else {
        location.column++;
    }

    if (ch != std::char_traits<char>::eof()) {
        curPos++;
        // 优化项： 当curPos到达buffer一半时
        if (curPos >= buffer.size() / 2) {
            size_t remainPos = endPos - curPos;
            std::copy(buffer.begin() + curPos, buffer.begin() + endPos, buffer.begin());

            streamPos += curPos;
            curPos = 0;
            endPos = remainPos;
        }
    }
    return ch;
}

char Source::peek(size_t n)
{
    if (eof()) {
        return std::char_traits<char>::eof();
    }
    fillBuffer(n);
    if (curPos + n >= endPos) {
        return std::char_traits<char>::eof();
    }
    return buffer[curPos+n];
}

void SourceBuffer::mark()
{
    segment.clear();
    location = buffer.pos();
}

char SourceBuffer::curch()
{
    if (segment.empty()) {
        return EOF;
    }
    return segment.at(segment.size() - 1);
}

char SourceBuffer::nextch()
{
    char ch = buffer.read();
    segment.push_back(ch);
    return ch;
}

char SourceBuffer::peek(size_t n)
{
    return buffer.peek(n);
}

bool SourceBuffer::match(char ch)
{
    if (peek() == ch) {
        nextch();
        return true;
    }
    return false;
}

std::string SourceBuffer::ObtainSegment() const
{
    return segment;
}

SourceLocation SourceBuffer::ObtainLocation() const
{
    return location;
}
