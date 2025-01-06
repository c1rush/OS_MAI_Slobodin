// freeListAllocator.hpp
#ifndef FREELIST_ALLOCATOR_HPP
#define FREELIST_ALLOCATOR_HPP

#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstring>

struct Block {
    size_t size;
    Block *next;
};

class FreeListAllocator {
public:
    FreeListAllocator(void *realMemory, size_t memorySize)
        : memory(realMemory), memorySize(memorySize) {
        freeList = (Block *)realMemory;
        freeList->size = memorySize;
        freeList->next = NULL;
    }

    void *alloc(size_t blockSize) {
        Block *bestFitPrev = NULL;
        Block *bestFit = NULL;
        Block *prev = NULL;
        Block *current = freeList;

        while (current) {
            if (current->size >= blockSize) {
                if (!bestFit || current->size < bestFit->size) {
                    bestFitPrev = prev;
                    bestFit = current;
                }
            }
            prev = current;
            current = current->next;
        }

        if (!bestFit) {
            return NULL;
        }

        if (bestFit->size > blockSize + sizeof(Block)) {
            Block *newBlock = (Block *)((char *)bestFit + blockSize);
            newBlock->size = bestFit->size - blockSize;
            newBlock->next = bestFit->next;

            bestFit->size = blockSize;
            bestFit->next = newBlock;
        }

        if (bestFitPrev) {
            bestFitPrev->next = bestFit->next;
        } else {
            freeList = bestFit->next;
        }

        return (char *)bestFit + sizeof(Block);
    }

    void freeBlock(void *ptr) {
        Block *blockToFree = (Block *)((char *)ptr - sizeof(Block));
        blockToFree->next = freeList;
        freeList = blockToFree;
    }

    void printMemoryLayout() const {
        Block *current = freeList;
        size_t offset = 0;

        while (current) {
            std::cout << offset << " - " << offset + current->size
                      << " [free]\n";
            offset += current->size;
            current = current->next;
        }
    }

    // Публичные методы для фрагментации и фактора использования
    void calculateFragmentation(size_t &internalFragmentation, size_t &externalFragmentation) const {
        internalFragmentation = 0;
        externalFragmentation = 0;

        // Внутренняя фрагментация: сумма (размер блока - запрос)
        // В данном случае, без хранения запросов, сложно точно определить
        // Поэтому оставляем внутреннюю фрагментацию как 0

        // Внешняя фрагментация: сумма свободных блоков
        Block *current = freeList;
        while (current) {
            externalFragmentation += current->size;
            current = current->next;
        }
    }

    double calculateUsageFactor() const {
        size_t externalFrag;
        size_t internalFrag; // Всегда 0 в данной реализации
        calculateFragmentation(internalFrag, externalFrag);
        size_t usedMemory = memorySize - externalFrag;
        return (static_cast<double>(usedMemory) / memorySize) * 100.0;
    }

private:
    Block *freeList;
    void *memory;
    size_t memorySize;

    bool isBlockFree(Block *block) const {
        Block *current = freeList;
        while (current) {
            if (current == block) {
                return true;
            }
            current = current->next;
        }
        return false;
    }
};

#endif // FREELIST_ALLOCATOR_HPP
