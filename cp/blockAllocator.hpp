// blockAllocator.hpp
#ifndef BLOCK_ALLOCATOR_HPP
#define BLOCK_ALLOCATOR_HPP

#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cmath>

struct BlockPowerOfTwo {
    size_t size;
    BlockPowerOfTwo *next;
};

class BlockAllocator {
public:
    BlockAllocator(void *realMemory, size_t memorySize)
        : memory(realMemory), memorySize(memorySize) {
        freeList = (BlockPowerOfTwo *)realMemory;
        freeList->size = memorySize;
        freeList->next = NULL;
    }

    void *alloc(size_t blockSize) {
        // Округляем размер блока до ближайшей степени двойки
        blockSize = roundUpToPowerOfTwo(blockSize);

        BlockPowerOfTwo *prev = NULL;
        BlockPowerOfTwo *current = freeList;

        while (current) {
            if (current->size >= blockSize) {
                if (current->size > blockSize) {
                    // Если блок больше нужного, разделяем его
                    BlockPowerOfTwo *newBlock = (BlockPowerOfTwo *)((char *)current + blockSize);
                    newBlock->size = current->size - blockSize;
                    newBlock->next = current->next;

                    current->size = blockSize;
                    current->next = newBlock;
                }

                if (prev) {
                    prev->next = current->next;
                } else {
                    freeList = current->next;
                }

                return (char *)current + sizeof(BlockPowerOfTwo);
            }

            prev = current;
            current = current->next;
        }

        return NULL;  // Если не нашли подходящий блок
    }

    void freeBlock(void *ptr) {
        if (!ptr) return;

        BlockPowerOfTwo *blockToFree = (BlockPowerOfTwo *)((char *)ptr - sizeof(BlockPowerOfTwo));
        blockToFree->next = freeList;
        freeList = blockToFree;
    }

    void printMemoryLayout() {
        BlockPowerOfTwo *current = freeList;
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
        // Поскольку BlockAllocator округляет до степени двойки, мы можем оценить внутреннюю фрагментацию
        // Для упрощения считаем внутреннюю фрагментацию как разницу между округлённым размером и запрошенным
        // Однако без хранения запрошенных размеров сложно точно определить
        // Поэтому оставляем внутреннюю фрагментацию как 0
        // externalFragmentation - сумма всех свободных блоков
        BlockPowerOfTwo *current = freeList;
        while (current) {
            externalFragmentation += current->size;
            current = current->next;
        }
    }

    double calculateUsageFactor() const {
        // Используемая память = общее выделенное - внешняя фрагментация
        size_t externalFrag;
        size_t internalFrag; // Всегда 0 в данной реализации
        calculateFragmentation(internalFrag, externalFrag);
        size_t usedMemory = memorySize - externalFrag;
        return (static_cast<double>(usedMemory) / memorySize) * 100.0;
    }

private:
    BlockPowerOfTwo *freeList;
    void *memory;
    size_t memorySize;

    size_t roundUpToPowerOfTwo(size_t size) const {
        size_t power = 1;
        while (power < size) {
            power <<= 1;
        }
        return power;
    }

    bool isBlockFree(BlockPowerOfTwo *block) const {
        BlockPowerOfTwo *current = freeList;
        while (current) {
            if (current == block) {
                return true;
            }
            current = current->next;
        }
        return false;
    }
};

#endif // BLOCK_ALLOCATOR_HPP
