#include "blockAllocator.hpp"
#include "freeListAllocator.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>

int main() {
    const size_t memorySize = 1024 * 1024 * 32; // 32 MB
    const size_t maxBlockSize = 1024 * 8;       // 8 KB максимальный размер блока
    const size_t numAllocations = 1000000;       // Количество аллокаций

    void* blockMemory = malloc(memorySize);      // Для аллокатора блоков 2^n
    void* freeListMemory = malloc(memorySize);   // Для Free List Allocator

    if (!blockMemory || !freeListMemory) {
        std::cerr << "Не удалось выделить память для аллокаторов." << std::endl;
        return 1;
    }

    BlockAllocator blockAllocator(blockMemory, memorySize); 
    FreeListAllocator freeListAllocator(freeListMemory, memorySize);

    std::vector<size_t> allocationSizes;
    allocationSizes.reserve(numAllocations);

    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(16, maxBlockSize);
    for (size_t i = 0; i < numAllocations; ++i) {
        allocationSizes.push_back(dist(rng));
    }

    std::vector<void*> blockPointers;
    blockPointers.reserve(numAllocations);
    std::vector<void*> freeListPointers;
    freeListPointers.reserve(numAllocations);

    size_t allocationCount = 0;
    size_t deallocationCount = 0;

    size_t blockTotalAllocated = 0;
    size_t freeListTotalAllocated = 0;

    // Benchmark для Block Allocator (2^n)
    std::cout << "Запуск Benchmark для Block Allocator 2^n..." << std::endl;
    auto startBlockAlloc = std::chrono::high_resolution_clock::now();
    auto totalBlockAllocTime = std::chrono::duration<double, std::micro>::zero();
    auto totalBlockFreeTime = std::chrono::duration<double, std::micro>::zero();

    for (size_t i = 0; i < allocationSizes.size(); ++i) {
        auto allocStart = std::chrono::high_resolution_clock::now();
        void* ptr = blockAllocator.alloc(allocationSizes[i]);
        auto allocEnd = std::chrono::high_resolution_clock::now();

        if (ptr == NULL) {
            std::cerr << "Block Allocator: Allocation failed at iteration " << i << std::endl;
            break;
        }

        blockPointers.push_back(ptr);
        allocationCount++;
        blockTotalAllocated += allocationSizes[i];
        totalBlockAllocTime += allocEnd - allocStart;

        if (allocationCount % 2 == 0) {
            size_t deallocIndex = allocationCount - 2;
            if (deallocIndex < blockPointers.size() && blockPointers[deallocIndex] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                blockAllocator.freeBlock(blockPointers[deallocIndex]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                totalBlockFreeTime += freeEnd - freeStart;
                blockTotalAllocated -= allocationSizes[deallocIndex];
                blockPointers[deallocIndex] = nullptr;
                deallocationCount++;
            }
        }
    }

    double blockUtilization = static_cast<double>(blockTotalAllocated) / memorySize * 100.0;

    // Освобождение оставшихся блоков для Block Allocator
    for (size_t i = 0; i < blockPointers.size(); ++i) {
        if (blockPointers[i] != nullptr) {
            auto freeStart = std::chrono::high_resolution_clock::now();
            blockAllocator.freeBlock(blockPointers[i]);
            auto freeEnd = std::chrono::high_resolution_clock::now();

            totalBlockFreeTime += freeEnd - freeStart;
            blockTotalAllocated -= allocationSizes[i];
            deallocationCount++;
            blockPointers[i] = nullptr;
        }
    }

    auto endBlockAlloc = std::chrono::high_resolution_clock::now();

    // Benchmark для Free List Allocator
    std::cout << "Запуск Benchmark для Free List Allocator..." << std::endl;

    auto startFreeListAlloc = std::chrono::high_resolution_clock::now();
    auto totalFreeListAllocTime = std::chrono::duration<double, std::micro>::zero();
    auto totalFreeListFreeTime = std::chrono::duration<double, std::micro>::zero();

    allocationCount = 0;
    deallocationCount = 0;
    freeListTotalAllocated = 0;

    for (size_t i = 0; i < allocationSizes.size(); ++i) {
        auto allocStart = std::chrono::high_resolution_clock::now();
        void* ptr = freeListAllocator.alloc(allocationSizes[i]);
        auto allocEnd = std::chrono::high_resolution_clock::now();

        if (ptr == NULL) {
            std::cerr << "Free List Allocator: Allocation failed at iteration " << i << std::endl;
            break;
        }

        freeListPointers.push_back(ptr);
        allocationCount++;
        freeListTotalAllocated += allocationSizes[i];
        totalFreeListAllocTime += allocEnd - allocStart;

        if (allocationCount % 2 == 0) {
            size_t deallocIndex = allocationCount - 2;
            if (deallocIndex < freeListPointers.size() && freeListPointers[deallocIndex] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                freeListAllocator.freeBlock(freeListPointers[deallocIndex]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                totalFreeListFreeTime += freeEnd - freeStart;
                freeListTotalAllocated -= allocationSizes[deallocIndex];
                freeListPointers[deallocIndex] = nullptr;
                deallocationCount++;
            }
        }
    }

    double freeListUtilization = static_cast<double>(freeListTotalAllocated) / memorySize * 100.0;

    // Освобождение оставшихся блоков для Free List Allocator
    for (size_t i = 0; i < freeListPointers.size(); ++i) {
        if (freeListPointers[i] != nullptr) {
            auto freeStart = std::chrono::high_resolution_clock::now();
            freeListAllocator.freeBlock(freeListPointers[i]);
            auto freeEnd = std::chrono::high_resolution_clock::now();

            totalFreeListFreeTime += freeEnd - freeStart;
            freeListTotalAllocated -= allocationSizes[i];
            deallocationCount++;
            freeListPointers[i] = nullptr;
        }
    }

    auto endFreeListAlloc = std::chrono::high_resolution_clock::now();

    // Расчет времени в миллисекундах
    double blockAllocTimeMs = totalBlockAllocTime.count() / 1000.0;
    double blockFreeTimeMs = totalBlockFreeTime.count() / 1000.0;

    double freeListAllocTimeMs = totalFreeListAllocTime.count() / 1000.0;
    double freeListFreeTimeMs = totalFreeListFreeTime.count() / 1000.0;

    // Расчет среднего времени в микросекундах
    double avgBlockAllocUs = (allocationCount > 0) ? (totalBlockAllocTime.count() / allocationCount) : 0.0;
    double avgBlockFreeUs = (deallocationCount > 0) ? (totalBlockFreeTime.count() / deallocationCount) : 0.0;

    double avgFreeListAllocUs = (allocationCount > 0) ? (totalFreeListAllocTime.count() / allocationCount) : 0.0;
    double avgFreeListFreeUs = (deallocationCount > 0) ? (totalFreeListFreeTime.count() / deallocationCount) : 0.0;

    // Вывод результатов Benchmark
    std::cout << "\nРезультаты Benchmark:\n\n";

    std::cout << "Block Allocator 2^n:\n";
    std::cout << "  Количество выделений: " << allocationCount << "\n";
    std::cout << "  Количество освобождений: " << deallocationCount << "\n";
    std::cout << "  Общее время выделения: " << blockAllocTimeMs << " ms (Среднее: "
              << avgBlockAllocUs << " μs)\n";
    std::cout << "  Общее время освобождения: " << blockFreeTimeMs << " ms (Среднее: "
              << avgBlockFreeUs << " μs)\n";
    std::cout << "  Фактор использования: " << blockUtilization << " %\n\n";

    std::cout << "Free List Allocator:\n";
    std::cout << "  Количество выделений: " << allocationCount << "\n";
    std::cout << "  Количество освобождений: " << deallocationCount << "\n";
    std::cout << "  Общее время выделения: " << freeListAllocTimeMs << " ms (Среднее: "
              << avgFreeListAllocUs << " μs)\n";
    std::cout << "  Общее время освобождения: " << freeListFreeTimeMs << " ms (Среднее: "
              << avgFreeListFreeUs << " μs)\n";
    std::cout << "  Фактор использования: " << freeListUtilization << " %\n\n";

    // Освобождение выделенной памяти
    free(blockMemory);
    free(freeListMemory);

    return 0; // Завершаем программу
}
