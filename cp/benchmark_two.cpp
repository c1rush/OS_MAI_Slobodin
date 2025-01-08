// benchmark_four.cpp
#include "blockAllocator.hpp"
#include "freeListAllocator.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>

int main() {
    // Параметры
    const size_t memorySize = 1024 * 1024 * 256; // 256 MB
    const size_t fixedBlockSize = 256;           // Фиксированный размер блока в байтах
    const size_t totalAllocations = 100000;      // Общее количество аллокаций
    const size_t measurementInterval = 10000;    // Интервал измерений

    // Инициализация аллокаторов
    void* blockMemory = malloc(memorySize);
    void* freeListMemory = malloc(memorySize);

    if (!blockMemory || !freeListMemory) {
        std::cerr << "Не удалось выделить память для аллокаторов." << std::endl;
        return 1;
    }

    BlockAllocator blockAllocator(blockMemory, memorySize);
    FreeListAllocator freeListAllocator(freeListMemory, memorySize);

    // Открытие CSV файла
    std::ofstream ofs("benchmark_four.csv");
    if (!ofs.is_open()) {
        std::cerr << "Не удалось открыть benchmark_four.csv для записи." << std::endl;
        free(blockMemory);
        free(freeListMemory);
        return 1;
    }

    // Запись заголовка
    ofs << "AllocatorType,AllocationCount,BlockSize,TotalAllocTime_us,AvgAllocTime_us\n";

    // Векторы для хранения указателей
    std::vector<void*> blockPointers;
    std::vector<void*> freeListPointers;
    blockPointers.reserve(totalAllocations);
    freeListPointers.reserve(totalAllocations);

    std::cout << "Benchmark Four: Измерение времени выделения памяти по количеству аллокаций с переиспользованием блоков." << std::endl;

    // Аллокатор BlockAllocator
    std::cout << "BlockAllocator: Начало выделения..." << std::endl;
    auto startBlockAlloc = std::chrono::high_resolution_clock::now();
    size_t successfulAllocsBlock = 0;

    for (size_t i = 1; i <= totalAllocations; ++i) {
        auto allocStart = std::chrono::high_resolution_clock::now();
        void* ptr = blockAllocator.alloc(fixedBlockSize);
        auto allocEnd = std::chrono::high_resolution_clock::now();

        if (ptr == nullptr) {
            std::cerr << "Block Allocator: Allocation failed at iteration " << i << std::endl;
            break;
        }

        blockPointers.push_back(ptr);
        successfulAllocsBlock++;

        // Промежуточное измерение и деаллокация для переиспользования блоков
        if (i % (measurementInterval * 2) == 0) {
            // Освобождаем половину блоков для переиспользования
            size_t deallocIndex = i / 2 - 1;
            if (deallocIndex < blockPointers.size() && blockPointers[deallocIndex] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                blockAllocator.freeBlock(blockPointers[deallocIndex]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, std::micro> freeDuration = freeEnd - freeStart;
                // Можно учесть время деаллокации, если необходимо

                blockPointers[deallocIndex] = nullptr;
            }
        }

        // Промежуточное измерение
        if (i % measurementInterval == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = currentTime - startBlockAlloc;
            double totalTime = duration.count();
            double avgTime = totalTime / i;

            // Запись в CSV
            ofs << "BlockAllocator," << i << "," << fixedBlockSize << "," << totalTime << "," << avgTime << "\n";

            std::cout << "BlockAllocator: " << i << " аллокаций выполнено. Общее время: " << totalTime << " мкс, Среднее время: " << avgTime << " мкс." << std::endl;
        }
    }
    auto endBlockAlloc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> totalDurationBlockAlloc = endBlockAlloc - startBlockAlloc;
    std::cout << "BlockAllocator: Завершено выделение " << successfulAllocsBlock << " блоков. Общее время: " << totalDurationBlockAlloc.count() << " мкс." << std::endl;

    // Освобождение памяти для BlockAllocator
    for (auto ptr : blockPointers) {
        if (ptr != nullptr) {
            blockAllocator.freeBlock(ptr);
        }
    }

    // Аллокатор FreeListAllocator
    std::cout << "FreeListAllocator: Начало выделения..." << std::endl;
    auto startFreeListAlloc = std::chrono::high_resolution_clock::now();
    size_t successfulAllocsFreeList = 0;

    for (size_t i = 1; i <= totalAllocations; ++i) {
        auto allocStart = std::chrono::high_resolution_clock::now();
        void* ptr = freeListAllocator.alloc(fixedBlockSize);
        auto allocEnd = std::chrono::high_resolution_clock::now();

        if (ptr == nullptr) {
            std::cerr << "Free List Allocator: Allocation failed at iteration " << i << std::endl;
            break;
        }

        freeListPointers.push_back(ptr);
        successfulAllocsFreeList++;

        // Промежуточное измерение и деаллокация для переиспользования блоков
        if (i % (measurementInterval * 2) == 0) {
            // Освобождаем половину блоков для переиспользования
            size_t deallocIndex = i / 2 - 1;
            if (deallocIndex < freeListPointers.size() && freeListPointers[deallocIndex] != nullptr) {
                auto freeStart = std::chrono::high_resolution_clock::now();
                freeListAllocator.freeBlock(freeListPointers[deallocIndex]);
                auto freeEnd = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, std::micro> freeDuration = freeEnd - freeStart;
                // Можно учесть время деаллокации, если необходимо

                freeListPointers[deallocIndex] = nullptr;
            }
        }

        // Промежуточное измерение
        if (i % measurementInterval == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = currentTime - startFreeListAlloc;
            double totalTime = duration.count();
            double avgTime = totalTime / i;

            // Запись в CSV
            ofs << "FreeListAllocator," << i << "," << fixedBlockSize << "," << totalTime << "," << avgTime << "\n";

            std::cout << "FreeListAllocator: " << i << " аллокаций выполнено. Общее время: " << totalTime << " мкс, Среднее время: " << avgTime << " мкс." << std::endl;
        }
    }
    auto endFreeListAlloc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> totalDurationFreeListAlloc = endFreeListAlloc - startFreeListAlloc;
    std::cout << "FreeListAllocator: Завершено выделение " << successfulAllocsFreeList << " блоков. Общее время: " << totalDurationFreeListAlloc.count() << " мкс." << std::endl;

    // Освобождение памяти для FreeListAllocator
    for (auto ptr : freeListPointers) {
        if (ptr != nullptr) {
            freeListAllocator.freeBlock(ptr);
        }
    }

    ofs.close();
    std::cout << "Benchmark Four завершён. Результаты записаны в benchmark_four.csv" << std::endl;

    // Освобождение выделенной памяти
    free(blockMemory);
    free(freeListMemory);

    return 0;
}
