#pragma once

#include <cstddef>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>

class BuddyAllocator {
private:
    void* memoryStart;                   // Начало выделенной памяти
    size_t totalSize;                    // Общий размер памяти
    size_t minBlockSize;                 // Минимальный размер блока
    int maxOrder;                        // Максимальный order
    std::vector<std::vector<size_t>> freeLists; // Списки свободных блоков по order

    int findOrder(size_t size) const;
    void mergeBlock(int order, size_t index);

    void printFreeList() const;

public:
    BuddyAllocator(void* memory, size_t size, size_t minBlockSize);
    ~BuddyAllocator();

    void* allocate(size_t size);
    void deallocate(void* ptr);

    size_t getLargestFreeBlock() const;
    size_t getTotalFreeMemory() const;
};
