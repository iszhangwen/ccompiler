#include "arena.h"

#include <cstdint>
#include <new>
#include <algorithm>

Arena::Arena(std::size_t chunkSize)
: m_chunkSize(chunkSize), m_current(nullptr), m_end(nullptr)
{
    allocateNewChunk();
}

Arena::~Arena()
{
    release();
}

void* Arena::allocate(std::size_t bytes, std::size_t alignment)
{
    // 计算对齐后的起始地址
    // 计算方法：向上取整到最近的倍数 ceil(current / alignment) * alignment
    auto alignedPtr = reinterpret_cast<char*>(
        (reinterpret_cast<uintptr_t>(m_current) + alignment - 1) & ~(alignment - 1)
    );

    // 需要的字节
    auto needed = static_cast<std::size_t>(alignedPtr - m_current) + bytes;
    if (needed > static_cast<std::size_t>(m_end - m_current)) {
        allocateNewChunk(bytes);
        // 重新对齐
        alignedPtr = reinterpret_cast<char*>(
            (reinterpret_cast<uintptr_t>(m_current) + alignment - 1) & ~(alignment - 1)
        );
    }

    // 分配完成后返回
    m_current = alignedPtr + bytes;
    return alignedPtr;
}

void Arena::release()
{
    for (auto chunk: m_chunks) {
        delete[] chunk;
    }
    m_current = nullptr;
    m_end = nullptr;
}

void Arena::allocateNewChunk(std::size_t minSize)
{
    auto size = std::max(m_chunkSize, minSize);
    auto chunk = new(std::nothrow) char[size];
    if (!chunk) {
        throw std::bad_alloc{};
    }
    m_chunks.push_back(chunk);
    m_current = chunk;
    m_end = chunk + size;
}
