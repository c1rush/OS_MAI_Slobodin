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
        blockSize = roundUpToPowerOfTwo(blockSize);

        BlockPowerOfTwo *prev = NULL;
        BlockPowerOfTwo *current = freeList;

        while (current) {
            if (current->size >= blockSize) {
                if (current->size > blockSize) {
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

        return NULL;
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

    void calculateFragmentation(size_t &internalFragmentation, size_t &externalFragmentation) const {
        internalFragmentation = 0;
        externalFragmentation = 0;

        BlockPowerOfTwo *current = freeList;
        while (current) {
            externalFragmentation += current->size;
            current = current->next;
        }
    }

    double calculateUsageFactor() const {
        size_t externalFrag;
        calculateFragmentation(/*internalFragmentation=*/std::ignore, externalFrag);
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
