#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define MIN_BLOCK_SIZE 32

class ListAllocator {
private:
    struct Block {
        size_t size;       // 8 байт
        Block* next;       // 8 байт
        bool is_free;      // 1 байт
        char padding[15];  // 15 байт паддинга для выравнивания до 32 байт
    };

    // Проверка размера структуры Block
    static_assert(sizeof(Block) == 32, "Block size must be 32 bytes.");

    Block* free_list;       // Указатель на список свободных блоков
    void* memory_start;     // Указатель на начало выделенной памяти
    size_t total_size;      // Общий размер выделенной памяти

    // Вспомогательные методы для отладки
    void printFreeList() const;

public:
    ListAllocator(void* memory, size_t size);
    ~ListAllocator();

    void* allocate(size_t size);
    void deallocate(void* ptr);

    size_t getLargestFreeBlock() const;
    size_t getTotalFreeMemory() const;
};
