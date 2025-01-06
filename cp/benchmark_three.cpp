// benchmark_three.cpp
#include "blockAllocator.hpp" 
#include "freeListAllocator.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstddef>   

struct Operation {
    bool isAllocation; // true - аллокация, false - деаллокация
    size_t size;        // Размер блока для аллокации (игнорируется при деаллокации)
};

// Функция для генерации списка операций
std::vector<Operation> generateOperations(size_t numOperations, double allocProbability, size_t maxBlockSize, std::mt19937 &rng) {
    std::vector<Operation> operations;
    operations.reserve(numOperations);
    std::uniform_int_distribution<size_t> sizeDist(16, maxBlockSize);
    std::uniform_real_distribution<double> actionDist(0.0, 1.0);

    for (size_t i = 0; i < numOperations; ++i) {
        double action = actionDist(rng);
        if (action < allocProbability) {
            // Аллокация
            size_t blockSize = sizeDist(rng);
            operations.push_back(Operation{true, blockSize});
        } else {
            // Деаллокация
            operations.push_back(Operation{false, 0});
        }
    }
    return operations;
}

int main() {
    // Параметры
    const size_t memorySize = 1024 * 1024 * 32; // 32 MB
    const size_t maxBlockSize = 1024 * 8;       // 8 KB максимальный размер блока
    const size_t numOperations = 100000;        // Количество операций (аллокация/деаллокация)
    const double allocProbability = 0.6;        // 60% вероятность аллокации

    // Инициализация аллокаторов
    void* blockMemory = malloc(memorySize);
    void* freeListMemory = malloc(memorySize);

    if (!blockMemory || !freeListMemory) {
        std::cerr << "Не удалось выделить память для аллокаторов." << std::endl;
        return 1;
    }

    BlockAllocator blockAllocator(blockMemory, memorySize);           // Новый аллокатор блоков 2^n
    FreeListAllocator freeListAllocator(freeListMemory, memorySize); // Free List Allocator

    // Открытие CSV файла для записи результатов
    std::ofstream ofs("benchmark_three.csv");
    if (!ofs.is_open()) {
        std::cerr << "Не удалось открыть benchmark_three.csv для записи." << std::endl;
        free(blockMemory);
        free(freeListMemory);
        return 1;
    }

    // Запись заголовка в CSV
    ofs << "AllocatorType,Iteration,InternalFragmentation_bytes,ExternalFragmentation_bytes,UsageFactor_percent\n";

    // Генерация списка операций
    std::mt19937 rng(42); // Фиксированный seed для воспроизводимости
    std::vector<Operation> operations = generateOperations(numOperations, allocProbability, maxBlockSize, rng);

    // Векторы для хранения указателей отдельно для каждого аллокатора
    std::vector<void*> blockPointers;
    std::vector<void*> freeListPointers;
    blockPointers.reserve(numOperations);
    freeListPointers.reserve(numOperations);

    // Параметры для вывода прогресса
    const size_t progressInterval = 10000;

    for (size_t i = 0; i < operations.size(); ++i) {
        const Operation &op = operations[i];

        if (op.isAllocation) {
            // Аллокация
            // -----------------------
            // Block Allocator
            // -----------------------
            void* ptrBlock = blockAllocator.alloc(op.size);
            if (ptrBlock != nullptr) {
                blockPointers.push_back(ptrBlock);
            } else {
                std::cerr << "Block Allocator: Allocation failed at iteration " << (i + 1) << std::endl;
            }

            // -----------------------
            // Free List Allocator
            // -----------------------
            void* ptrFreeList = freeListAllocator.alloc(op.size);
            if (ptrFreeList != nullptr) {
                freeListPointers.push_back(ptrFreeList);
            } else {
                std::cerr << "Free List Allocator: Allocation failed at iteration " << (i + 1) << std::endl;
            }
        } else {
            // Деаллокация (LIFO)
            // -----------------------
            // Block Allocator
            // -----------------------
            if (!blockPointers.empty()) {
                void* ptr = blockPointers.back();
                if (ptr != nullptr) {
                    blockAllocator.freeBlock(ptr);
                    blockPointers.pop_back();
                }
            }

            // -----------------------
            // Free List Allocator
            // -----------------------
            if (!freeListPointers.empty()) {
                void* ptr = freeListPointers.back();
                if (ptr != nullptr) {
                    freeListAllocator.freeBlock(ptr);
                    freeListPointers.pop_back();
                }
            }
        }

        // Каждые progressInterval итераций записываем состояние
        if ((i + 1) % progressInterval == 0) {
            size_t internalFragBlock = 0;
            size_t externalFragBlock = 0;
            double usageFactorBlock = 0.0;

            // -----------------------
            // Block Allocator
            // -----------------------
            blockAllocator.calculateFragmentation(internalFragBlock, externalFragBlock);
            usageFactorBlock = blockAllocator.calculateUsageFactor();

            ofs << "BlockAllocator," << (i + 1) << "," << internalFragBlock << "," << externalFragBlock << "," << usageFactorBlock << "\n";

            size_t internalFragFreeList = 0;
            size_t externalFragFreeList = 0;
            double usageFactorFreeList = 0.0;

            // -----------------------
            // Free List Allocator
            // -----------------------
            freeListAllocator.calculateFragmentation(internalFragFreeList, externalFragFreeList);
            usageFactorFreeList = freeListAllocator.calculateUsageFactor();

            ofs << "FreeListAllocator," << (i + 1) << "," << internalFragFreeList << "," << externalFragFreeList << "," << usageFactorFreeList << "\n";

            // Вывод прогресса в консоль
            std::cout << "Итерация " << (i + 1) << " из " << numOperations << " завершена." << std::endl;
            std::cout << "  BlockAllocator: Internal Frag = " << internalFragBlock 
                      << " bytes, External Frag = " << externalFragBlock 
                      << " bytes, Usage = " << usageFactorBlock << " %" << std::endl;
            std::cout << "  FreeListAllocator: Internal Frag = " << internalFragFreeList 
                      << " bytes, External Frag = " << externalFragFreeList 
                      << " bytes, Usage = " << usageFactorFreeList << " %" << std::endl;
        }
    }

    // Запись состояния после завершения всех операций, если последние итерации не кратны progressInterval
    if (numOperations % progressInterval != 0) {
        size_t internalFragBlock = 0;
        size_t externalFragBlock = 0;
        double usageFactorBlock = 0.0;

        // -----------------------
        // Block Allocator
        // -----------------------
        blockAllocator.calculateFragmentation(internalFragBlock, externalFragBlock);
        usageFactorBlock = blockAllocator.calculateUsageFactor();

        ofs << "BlockAllocator," << numOperations << "," << internalFragBlock << "," << externalFragBlock << "," << usageFactorBlock << "\n";

        size_t internalFragFreeList = 0;
        size_t externalFragFreeList = 0;
        double usageFactorFreeList = 0.0;

        // -----------------------
        // Free List Allocator
        // -----------------------
        freeListAllocator.calculateFragmentation(internalFragFreeList, externalFragFreeList);
        usageFactorFreeList = freeListAllocator.calculateUsageFactor();

        ofs << "FreeListAllocator," << numOperations << "," << internalFragFreeList << "," << externalFragFreeList << "," << usageFactorFreeList << "\n";

        // Вывод прогресса в консоль
        std::cout << "Итерация " << numOperations << " из " << numOperations << " завершена." << std::endl;
        std::cout << "  BlockAllocator: Internal Frag = " << internalFragBlock 
                  << " bytes, External Frag = " << externalFragBlock 
                  << " bytes, Usage = " << usageFactorBlock << " %" << std::endl;
        std::cout << "  FreeListAllocator: Internal Frag = " << internalFragFreeList 
                  << " bytes, External Frag = " << externalFragFreeList 
                  << " bytes, Usage = " << usageFactorFreeList << " %" << std::endl;
    }

    // Освобождение оставшейся памяти для BlockAllocator
    while (!blockPointers.empty()) {
        void* ptr = blockPointers.back();
        if (ptr != nullptr) {
            blockAllocator.freeBlock(ptr);
            blockPointers.pop_back();
        }
    }

    // Освобождение оставшейся памяти для FreeListAllocator
    while (!freeListPointers.empty()) {
        void* ptr = freeListPointers.back();
        if (ptr != nullptr) {
            freeListAllocator.freeBlock(ptr);
            freeListPointers.pop_back();
        }
    }

    ofs.close();
    std::cout << "Benchmark Three завершён. Результаты записаны в benchmark_three.csv" << std::endl;

    // Освобождение выделенной памяти
    free(blockMemory);
    free(freeListMemory);

    return 0; // Завершаем программу
}
