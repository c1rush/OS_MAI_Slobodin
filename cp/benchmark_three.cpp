// benchmark_five.cpp
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
    const size_t totalDeallocations = 100000;    // Общее количество деаллокаций
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
    std::ofstream ofs("benchmark_five.csv");
    if (!ofs.is_open()) {
        std::cerr << "Не удалось открыть benchmark_five.csv для записи." << std::endl;
        free(blockMemory);
        free(freeListMemory);
        return 1;
    }

    // Запись заголовка
    ofs << "AllocatorType,DeallocationCount,BlockSize,TotalFreeTime_us,AvgFreeTime_us\n";

    // Векторы для хранения указателей
    std::vector<void*> blockPointers;
    std::vector<void*> freeListPointers;
    blockPointers.reserve(totalDeallocations);
    freeListPointers.reserve(totalDeallocations);

    std::cout << "Benchmark Five: Предварительное выделение памяти для деаллокаций." << std::endl;

    // Предварительное выделение для BlockAllocator
    std::cout << "BlockAllocator: Начало предварительного выделения..." << std::endl;
    size_t successfulPreAllocsBlock = 0;
    for (size_t i = 0; i < totalDeallocations; ++i) {
        void* ptr = blockAllocator.alloc(fixedBlockSize);
        if (ptr == nullptr) {
            std::cerr << "Block Allocator: Allocation failed at pre-allocation " << i << std::endl;
            break;
        }
        blockPointers.push_back(ptr);
        successfulPreAllocsBlock++;
    }
    std::cout << "BlockAllocator: Завершено предварительное выделение " << successfulPreAllocsBlock << " блоков." << std::endl;

    // Предварительное выделение для FreeListAllocator
    std::cout << "FreeListAllocator: Начало предварительного выделения..." << std::endl;
    size_t successfulPreAllocsFreeList = 0;
    for (size_t i = 0; i < totalDeallocations; ++i) {
        void* ptr = freeListAllocator.alloc(fixedBlockSize);
        if (ptr == nullptr) {
            std::cerr << "Free List Allocator: Allocation failed at pre-allocation " << i << std::endl;
            break;
        }
        freeListPointers.push_back(ptr);
        successfulPreAllocsFreeList++;
    }
    std::cout << "FreeListAllocator: Завершено предварительное выделение " << successfulPreAllocsFreeList << " блоков." << std::endl;

    std::cout << "Benchmark Five: Начало деаллокации и измерения времени." << std::endl;

    // Аллокатор BlockAllocator
    std::cout << "BlockAllocator: Начало деаллокации..." << std::endl;
    auto startBlockFree = std::chrono::high_resolution_clock::now();
    size_t successfulDeallocsBlock = 0;

    for (size_t i = 1; i <= blockPointers.size(); ++i) {
        blockAllocator.freeBlock(blockPointers[i - 1]);
        successfulDeallocsBlock++;

        // Промежуточное измерение
        if (i % measurementInterval == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = currentTime - startBlockFree;
            double totalTime = duration.count();
            double avgTime = totalTime / i;

            // Запись в CSV
            ofs << "BlockAllocator," << i << "," << fixedBlockSize << "," << totalTime << "," << avgTime << "\n";

            std::cout << "BlockAllocator: " << i << " деаллокаций выполнено. Общее время: " << totalTime << " мкс, Среднее время: " << avgTime << " мкс." << std::endl;
        }
    }
    auto endBlockFree = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> totalDurationBlockFree = endBlockFree - startBlockFree;
    std::cout << "BlockAllocator: Завершено деаллокацию " << successfulDeallocsBlock << " блоков. Общее время: " << totalDurationBlockFree.count() << " мкс." << std::endl;

    // Аллокатор FreeListAllocator
    std::cout << "FreeListAllocator: Начало деаллокации..." << std::endl;
    auto startFreeListFree = std::chrono::high_resolution_clock::now();
    size_t successfulDeallocsFreeList = 0;

    for (size_t i = 1; i <= freeListPointers.size(); ++i) {
        freeListAllocator.freeBlock(freeListPointers[i - 1]);
        successfulDeallocsFreeList++;

        // Промежуточное измерение
        if (i % measurementInterval == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = currentTime - startFreeListFree;
            double totalTime = duration.count();
            double avgTime = totalTime / i;

            // Запись в CSV
            ofs << "FreeListAllocator," << i << "," << fixedBlockSize << "," << totalTime << "," << avgTime << "\n";

            std::cout << "FreeListAllocator: " << i << " деаллокаций выполнено. Общее время: " << totalTime << " мкс, Среднее время: " << avgTime << " мкс." << std::endl;
        }
    }
    auto endFreeListFree = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> totalDurationFreeListFree = endFreeListFree - startFreeListFree;
    std::cout << "FreeListAllocator: Завершено деаллокацию " << successfulDeallocsFreeList << " блоков. Общее время: " << totalDurationFreeListFree.count() << " мкс." << std::endl;

    ofs.close();
    std::cout << "Benchmark Five завершён. Результаты записаны в benchmark_five.csv" << std::endl;

    // Освобождение выделенной памяти
    free(blockMemory);
    free(freeListMemory);

    return 0;
}
