// benchmark_six.cpp
#include "blockAllocator.hpp" 
#include "freeListAllocator.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip> 
#include <cstdlib>

int main() {
    // Список разных объёмов памяти (в байтах)
    std::vector<size_t> memorySizes = {
        16UL * 1024UL * 1024UL,  // 16 MB
        32UL * 1024UL * 1024UL,  // 32 MB
        64UL * 1024UL * 1024UL,  // 64 MB
        128UL * 1024UL * 1024UL, // 128 MB
        256UL * 1024UL * 1024UL  // 256 MB
    };

    // Общие настройки для всех тестов
    const size_t maxBlockSize = 1024 * 8;  // 8 KB
    const size_t numAllocations = 1000000; // 1 млн аллокаций

    // Генерация случайных размеров блоков один раз для всех тестов
    std::vector<size_t> allocationSizes;
    allocationSizes.reserve(numAllocations);

    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(16, maxBlockSize);
    for (size_t i = 0; i < numAllocations; ++i) {
        allocationSizes.push_back(dist(rng));
    }

    // Цикл по списку объёмов памяти
    for (size_t memorySize : memorySizes) {
        size_t memInMB = memorySize / (1024UL * 1024UL);
        std::cout << "\n=== Тест с объёмом памяти: " << memInMB 
                  << " MB ===" << std::endl;

        // Выделяем память для двух аллокаторов
        void* blockMemory = malloc(memorySize);
        void* freeListMemory = malloc(memorySize);

        if (!blockMemory || !freeListMemory) {
            std::cerr << "Не удалось выделить память (" << memInMB 
                      << " MB) для аллокаторов." << std::endl;
            if (blockMemory) free(blockMemory);
            if (freeListMemory) free(freeListMemory);
            continue; // Переходим к следующему объёму
        }

        // Создаём объекты аллокаторов
        BlockAllocator blockAllocator(blockMemory, memorySize);
        FreeListAllocator freeListAllocator(freeListMemory, memorySize);

        // ----- Benchmark для Block Allocator (2^n) -----
        std::cout << "Запуск Benchmark для Block Allocator 2^n..." << std::endl;

        std::vector<void*> blockPointers;
        blockPointers.reserve(numAllocations);

        size_t allocationCount = 0;
        size_t deallocationCount = 0;
        size_t blockTotalAllocated = 0;

        auto totalBlockAllocTime = std::chrono::duration<double, std::micro>::zero();
        auto totalBlockFreeTime  = std::chrono::duration<double, std::micro>::zero();

        for (size_t i = 0; i < allocationSizes.size(); ++i) {
            auto allocStart = std::chrono::high_resolution_clock::now();
            void* ptr = blockAllocator.alloc(allocationSizes[i]);
            auto allocEnd = std::chrono::high_resolution_clock::now();

            if (!ptr) {
                std::cerr << "Block Allocator: Allocation failed at iteration " << i << std::endl;
                break;
            }

            blockPointers.push_back(ptr);
            allocationCount++;
            blockTotalAllocated += allocationSizes[i];
            totalBlockAllocTime += (allocEnd - allocStart);

            // Освобождение каждые две аллокации
            if (allocationCount % 2 == 0) {
                size_t deallocIndex = allocationCount - 2;
                if (deallocIndex < blockPointers.size() && blockPointers[deallocIndex] != nullptr) {
                    auto freeStart = std::chrono::high_resolution_clock::now();
                    blockAllocator.freeBlock(blockPointers[deallocIndex]);
                    auto freeEnd = std::chrono::high_resolution_clock::now();

                    totalBlockFreeTime += (freeEnd - freeStart);
                    blockTotalAllocated -= allocationSizes[deallocIndex];
                    blockPointers[deallocIndex] = nullptr;
                    deallocationCount++;
                }
            }
        }

        // Освобождение оставшихся блоков
        for (size_t i = 0; i < blockPointers.size(); ++i) {
            if (blockPointers[i] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                blockAllocator.freeBlock(blockPointers[i]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                totalBlockFreeTime += (freeEnd - freeStart);
                blockTotalAllocated -= allocationSizes[i];
                deallocationCount++;
                blockPointers[i] = nullptr;
            }
        }

        double blockAllocTimeMs  = totalBlockAllocTime.count() / 1000.0;
        double blockFreeTimeMs   = totalBlockFreeTime.count() / 1000.0;
        double avgBlockAllocUs   = (allocationCount > 0)
                                 ? (totalBlockAllocTime.count() / allocationCount)
                                 : 0.0;
        double avgBlockFreeUs    = (deallocationCount > 0)
                                 ? (totalBlockFreeTime.count() / deallocationCount)
                                 : 0.0;
        double blockUtilization  = static_cast<double>(blockTotalAllocated) 
                                 / memorySize * 100.0;

        std::cout << "\nРезультаты Benchmark (Block Allocator 2^n):\n\n";
        std::cout << "  Количество выделений:     " << allocationCount << "\n";
        std::cout << "  Количество освобождений:  " << deallocationCount << "\n";
        std::cout << "  Общее время выделения:    " << blockAllocTimeMs 
                  << " ms (Среднее: " << avgBlockAllocUs << " μs)\n";
        std::cout << "  Общее время освобождения: " << blockFreeTimeMs 
                  << " ms (Среднее: " << avgBlockFreeUs << " μs)\n";
        std::cout << "  Фактор использования:     " << blockUtilization << " %\n\n";

        // ----- Benchmark для Free List Allocator -----
        std::cout << "Запуск Benchmark для Free List Allocator..." << std::endl;

        std::vector<void*> freeListPointers;
        freeListPointers.reserve(numAllocations);

        allocationCount = 0;
        deallocationCount = 0;
        size_t freeListTotalAllocated = 0;

        auto totalFreeListAllocTime = std::chrono::duration<double, std::micro>::zero();
        auto totalFreeListFreeTime  = std::chrono::duration<double, std::micro>::zero();

        for (size_t i = 0; i < allocationSizes.size(); ++i) {
            auto allocStart = std::chrono::high_resolution_clock::now();
            void* ptr = freeListAllocator.alloc(allocationSizes[i]);
            auto allocEnd = std::chrono::high_resolution_clock::now();

            if (!ptr) {
                std::cerr << "Free List Allocator: Allocation failed at iteration " << i << std::endl;
                break;
            }

            freeListPointers.push_back(ptr);
            allocationCount++;
            freeListTotalAllocated += allocationSizes[i];
            totalFreeListAllocTime += (allocEnd - allocStart);

            if (allocationCount % 2 == 0) {
                size_t deallocIndex = allocationCount - 2;
                if (deallocIndex < freeListPointers.size() && freeListPointers[deallocIndex] != nullptr) {
                    auto freeStart = std::chrono::high_resolution_clock::now();
                    freeListAllocator.freeBlock(freeListPointers[deallocIndex]);
                    auto freeEnd = std::chrono::high_resolution_clock::now();

                    totalFreeListFreeTime += (freeEnd - freeStart);
                    freeListTotalAllocated -= allocationSizes[deallocIndex];
                    freeListPointers[deallocIndex] = nullptr;
                    deallocationCount++;
                }
            }
        }

        // Освобождаем оставшиеся блоки
        for (size_t i = 0; i < freeListPointers.size(); ++i) {
            if (freeListPointers[i] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                freeListAllocator.freeBlock(freeListPointers[i]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                totalFreeListFreeTime += (freeEnd - freeStart);
                freeListTotalAllocated -= allocationSizes[i];
                deallocationCount++;
                freeListPointers[i] = nullptr;
            }
        }

        double freeListAllocTimeMs  = totalFreeListAllocTime.count() / 1000.0;
        double freeListFreeTimeMs   = totalFreeListFreeTime.count() / 1000.0;
        double avgFreeListAllocUs   = (allocationCount > 0)
                                    ? (totalFreeListAllocTime.count() / allocationCount)
                                    : 0.0;
        double avgFreeListFreeUs    = (deallocationCount > 0)
                                    ? (totalFreeListFreeTime.count() / deallocationCount)
                                    : 0.0;
        double freeListUtilization  = static_cast<double>(freeListTotalAllocated) 
                                    / memorySize * 100.0;

        std::cout << "\nРезультаты Benchmark (Free List Allocator):\n\n";
        std::cout << "  Количество выделений:     " << allocationCount << "\n";
        std::cout << "  Количество освобождений:  " << deallocationCount << "\n";
        std::cout << "  Общее время выделения:    " << freeListAllocTimeMs 
                  << " ms (Среднее: " << avgFreeListAllocUs << " μs)\n";
        std::cout << "  Общее время освобождения: " << freeListFreeTimeMs 
                  << " ms (Среднее: " << avgFreeListFreeUs << " μs)\n";
        std::cout << "  Фактор использования:     " << freeListUtilization << " %\n\n";

        // Освобождаем и саму память, выделенную под аллокаторы
        free(blockMemory);
        free(freeListMemory);

        std::cout << "Тест c объёмом памяти " 
                  << memInMB << " MB завершён.\n";
    }

    std::cout << "\nВсе тесты benchmark_six завершены." << std::endl;
    return 0;
}
