#include <algorithm>
#include "buddy_allocator.hpp"

BuddyAllocator::BuddyAllocator(void* memory, size_t size, size_t minBlockSize_) {
    if (!memory || size < minBlockSize_) {
        throw std::invalid_argument("Invalid memory or size");
    }

    memoryStart = memory;
    totalSize = size;
    minBlockSize = minBlockSize_;

    // Вычисляем максимальный order
    maxOrder = 0;
    size_t blockSize = minBlockSize;
    while (blockSize < totalSize) {
        blockSize <<= 1; // Удваиваем размер блока
        maxOrder++;
    }

    // Инициализируем freeLists
    freeLists.resize(maxOrder + 1);
    freeLists[maxOrder].push_back(0); // Начинаем с одного большого блока

    // Обнуляем память для чистоты
    std::memset(memoryStart, 0, totalSize);

    std::cout << "[BuddyAllocator] Initialized with total size: " << totalSize 
              << " bytes, minBlockSize: " << minBlockSize 
              << " bytes, maxOrder: " << maxOrder << ".\n";
}

BuddyAllocator::~BuddyAllocator() {
    // В текущей реализации ничего не делаем, память управляется вне аллокатора
    std::cout << "[BuddyAllocator] Destructor called.\n";
}

int BuddyAllocator::findOrder(size_t size) const {
    // Учитываем размер для хранения order
    size += sizeof(int); // Добавляем место для хранения order

    int order = 0;
    size_t blockSize = minBlockSize;
    while (blockSize < size && order < maxOrder) {
        blockSize <<= 1; // Удваиваем размер блока
        order++;
    }
    return order; // Возвращаем требуемый order
}

void* BuddyAllocator::allocate(size_t size) {
    if (size == 0 || size > totalSize - sizeof(int)) {
        std::cout << "[BuddyAllocator] Allocation failed: Invalid size " << size << " bytes.\n";
        return nullptr;
    }

    // Определяем необходимый order
    int orderNeeded = findOrder(size);
    std::cout << "[BuddyAllocator] Allocating " << size << " bytes requires order " << orderNeeded << ".\n";

    // Ищем свободный блок подходящего размера или большего
    int currentOrder = orderNeeded;
    while (currentOrder <= maxOrder && freeLists[currentOrder].empty()) {
        currentOrder++;
    }

    if (currentOrder > maxOrder) {
        std::cout << "[BuddyAllocator] Allocation failed: No suitable block found for size " << size << " bytes.\n";
        return nullptr; // Не нашли подходящего блока
    }

    // Берём блок из списка freeLists
    size_t blockIndex = freeLists[currentOrder].back();
    freeLists[currentOrder].pop_back();
    std::cout << "[BuddyAllocator] Found free block at index " << blockIndex 
              << " on order " << currentOrder << ".\n";

    // Спускаемся вниз до нужного order, разбивая блоки
    while (currentOrder > orderNeeded) {
        currentOrder--;
        size_t buddyIndex = blockIndex * 2 + 1; // Индекс второго блока
        freeLists[currentOrder].push_back(buddyIndex);
        std::cout << "[BuddyAllocator] Split: added buddy index " << buddyIndex 
                  << " to order " << currentOrder << ".\n";
    }

    // Теперь blockIndex соответствует orderNeeded
    size_t blockSizeAllocated = minBlockSize << orderNeeded;
    size_t offset = blockIndex * blockSizeAllocated;

    // Адрес для хранения order внутри блока
    int* order_ptr = reinterpret_cast<int*>(static_cast<char*>(memoryStart) + offset);
    *order_ptr = orderNeeded;

    // Адрес, который будет возвращён пользователю (после order)
    void* allocated_ptr = static_cast<char*>(memoryStart) + offset + sizeof(int);
    std::cout << "[BuddyAllocator] Allocated block at " << allocated_ptr 
              << " with size " << blockSizeAllocated << " bytes.\n";

    printFreeList();

    return allocated_ptr; // Возвращаем указатель после order
}

void BuddyAllocator::deallocate(void* ptr) {
    if (!ptr) {
        std::cout << "[BuddyAllocator] Deallocation failed: Null pointer.\n";
        return; // Игнорируем
    }

    // Получаем указатель на order внутри блока
    int* order_ptr = reinterpret_cast<int*>(static_cast<char*>(ptr) - sizeof(int));
    int order = *order_ptr;

    std::cout << "[BuddyAllocator] Deallocating block at " << ptr 
              << ", order: " << order << ".\n";

    if (order < 0 || order > maxOrder) {
        std::cout << "[BuddyAllocator] Deallocation failed: Invalid order " << order << ".\n";
        return; // Некорректный order
    }

    size_t blockSize = minBlockSize << order;
    size_t offset = reinterpret_cast<char*>(order_ptr) - static_cast<char*>(memoryStart);
    size_t blockIndex = offset / blockSize;

    // Возвращаем блок в freeLists
    freeLists[order].push_back(blockIndex);
    std::cout << "[BuddyAllocator] Returned block index " << blockIndex 
              << " to free list of order " << order << ".\n";

    // Пытаемся слить блок с buddy
    mergeBlock(order, blockIndex);

    printFreeList();
}

void BuddyAllocator::mergeBlock(int order, size_t index) {
    if (order >= maxOrder) {
        std::cout << "[BuddyAllocator] Cannot merge: order " << order << " is already maxOrder.\n";
        return; // Дальше некуда сливать
    }

    // Вычисляем buddyIndex
    size_t buddyIndex = (index % 2 == 0) ? index + 1 : index - 1;
    std::cout << "[BuddyAllocator] Attempting to merge block index " << index 
              << " with buddy index " << buddyIndex << " on order " << order << ".\n";

    // Ищем buddy в freeLists
    auto& currentList = freeLists[order];
    auto it = std::find(currentList.begin(), currentList.end(), buddyIndex);
    if (it == currentList.end()) {
        std::cout << "[BuddyAllocator] Buddy index " << buddyIndex 
                  << " not found in free list of order " << order << ". Cannot merge.\n";
        return; // Buddy не свободен
    }

    // Удаляем buddy из freeLists
    currentList.erase(it);
    std::cout << "[BuddyAllocator] Buddy index " << buddyIndex 
              << " found and removed from free list.\n";

    // Вычисляем новый mergedIndex
    size_t mergedIndex = (index < buddyIndex) ? index : buddyIndex;

    // Добавляем слитый блок в freeLists на order + 1
    freeLists[order + 1].push_back(mergedIndex);
    std::cout << "[BuddyAllocator] Merged block index " << mergedIndex 
              << " added to free list of order " << (order + 1) << ".\n";

    // Рекурсивно пытаемся слить дальше
    mergeBlock(order + 1, mergedIndex);
}

size_t BuddyAllocator::getLargestFreeBlock() const {
    size_t largest = 0;
    // Пробегаемся по всем order начиная с большего
    for (int o = maxOrder; o >= 0; o--) {
        if (!freeLists[o].empty()) {
            size_t blockSize = minBlockSize << o;
            largest = blockSize;
            break;
        }
    }
    return largest;
}

size_t BuddyAllocator::getTotalFreeMemory() const {
    size_t total = 0;
    for (int o = 0; o <= maxOrder; o++) {
        size_t blockSize = minBlockSize << o;
        total += freeLists[o].size() * blockSize;
    }
    return total;
}

void BuddyAllocator::printFreeList() const {
    std::cout << "[BuddyAllocator] Free Lists:\n";
    for (int o = 0; o <= maxOrder; o++) {
        if (!freeLists[o].empty()) {
            std::cout << "  Order " << o << " (Block Size: " 
                      << (minBlockSize << o) << " bytes): ";
            for (const auto& index : freeLists[o]) {
                std::cout << index << " ";
            }
            std::cout << "\n";
        }
    }
}
