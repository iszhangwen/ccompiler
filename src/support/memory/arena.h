#pragma once
#include <vector>
#include <cstddef>

class Arena 
{
public:
    static Arena* Instance() {
        static Arena obj;
        return &obj;
    }
    ~Arena();

    // 禁止拷贝和移动操作
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    // 方便创建对象
    template<typename T, typename... Args>
    static T* make(Args&&... args) {
        static thread_local Arena arena;
        return new(arena) T(std::forward<Args>(args)...);
    }

    // 分配内存，自动对齐
    void* allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t));
    // 释放内存
    void release();

private:
    std::size_t m_chunkSize;
    std::vector<char*> m_chunks;

    // 记录可分配的内存区域
    char* m_current;
    char* m_end;

    explicit Arena(std::size_t chunkSize = 60 * 1024);
        // 分配新的chunk
    void allocateNewChunk(std::size_t minSize = 0);
};

class ArenaObject
{
public:
    virtual ~ArenaObject() = default;   
};

template <typename T>
class ArenaNode : public ArenaObject
{
public:
    virtual ~ArenaNode() = default;  
    void* operator new(std::size_t) = delete;
    void operator delete(void*) = delete;
    void* operator new(std::size_t size, Arena& arena) {return arena.allocate(size, alignof(T));}
    void operator delete(void*, Arena*) = delete;
};