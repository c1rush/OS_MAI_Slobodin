#include "list_allocator.hpp"

#include <cassert>

ListAllocator::ListAllocator(void* memory, size_t size) {
    if (!memory || size < sizeof(Block)) {
        throw std::invalid_argument("Invalid memory or size");
    }

    std::cout << "[ListAllocator] Size of Block: " << sizeof(Block) << " bytes.\n";

    memory_start = static_cast<char*>(memory);
    total_size   = size;

    free_list = reinterpret_cast<Block*>(memory_start);
    free_list->size    = total_size - sizeof(Block);
    free_list->next    = nullptr;
    free_list->is_free = true;

    std::cout << "[ListAllocator] Initialized with size: " << total_size << " bytes.\n";
    printFreeList();
}

ListAllocator::~ListAllocator() {
    if (memory_start) {
        std::memset(memory_start, 0, total_size);
        std::cout << "[ListAllocator] Memory cleared.\n";
    }
}

void ListAllocator::printFreeList() const {
    std::cout << "[ListAllocator] Free List:\n";
    Block* current = free_list;
    while (current) {
        std::cout << "  Block at " << current 
                  << ", size: " << current->size 
                  << ", is_free: " << current->is_free << "\n";
        current = current->next;
    }
}

void* ListAllocator::allocate(size_t size) {
    if (size == 0) {
        return nullptr;
    }

    // Выровняем размер до ближайшего кратного MIN_BLOCK_SIZE
    size = ((size + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE) * MIN_BLOCK_SIZE;

    Block* best = nullptr;
    Block* prev_best = nullptr;
    Block* current = free_list;
    Block* prev = nullptr;

    // Поиск best-fit
    while (current) {
        if (current->is_free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
                prev_best = prev;
            }
        }
        prev = current;
        current = current->next;
    }

    if (!best) {
        std::cout << "[ListAllocator] Allocation failed: No suitable block found for size " << size << " bytes.\n";
        return nullptr; // нет подходящего блока
    }

    std::cout << "[ListAllocator] Allocating " << size << " bytes from block at " << best << ".\n";

    // Сплит
    size_t remain_size = best->size - size;
    if (remain_size >= sizeof(Block) + MIN_BLOCK_SIZE) {
        Block* new_block = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(best) + sizeof(Block) + size
        );

        // Проверка, что new_block не выходит за пределы памяти
        size_t new_block_offset = reinterpret_cast<char*>(new_block) - static_cast<char*>(memory_start);
        assert(new_block_offset + sizeof(Block) <= total_size && "New block exceeds memory bounds!");

        new_block->size    = remain_size - sizeof(Block);
        new_block->is_free = true;
        new_block->next    = best->next;

        best->next = new_block;
        best->size = size;

        std::cout << "[ListAllocator] Split block: new_block at " << new_block 
                  << ", size: " << new_block->size << " bytes.\n";
    }

    best->is_free = false;

    // Вынимаем best из free_list
    if (prev_best == nullptr) {
        // best был головным
        free_list = best->next;
    } else {
        prev_best->next = best->next;
    }

    printFreeList();

    // Возвращаем адрес после заголовка
    std::cout << "[ListAllocator] Returning pointer: " 
              << reinterpret_cast<void*>(reinterpret_cast<char*>(best) + sizeof(Block)) 
              << "\n";
    return reinterpret_cast<void*>(
        reinterpret_cast<char*>(best) + sizeof(Block)
    );
}

void ListAllocator::deallocate(void* ptr) {
    if (!ptr) {
        return;
    }

    Block* block = reinterpret_cast<Block*>(
        reinterpret_cast<char*>(ptr) - sizeof(Block)
    );

    std::cout << "[ListAllocator] Deallocating block at " << block 
              << ", size: " << block->size << " bytes.\n";

    block->is_free = true;

    // Вставляем блок в отсортированный список свободных блоков
    if (!free_list) {
        free_list = block;
        block->next = nullptr;
    }
    else if (block < free_list) {
        block->next = free_list;
        free_list = block;
    }
    else {
        Block* current = free_list;
        while (current->next && current->next < block) {
            current = current->next;
        }
        block->next = current->next;
        current->next = block;
    }

    printFreeList();

    // Попытка слить смежные блоки
    Block* current = free_list;
    while (current && current->next) {
        char* endCurrent = reinterpret_cast<char*>(current)
                           + sizeof(Block) + current->size;
        char* nextBlockAddr = reinterpret_cast<char*>(current->next);
        bool can_merge = (endCurrent == nextBlockAddr);

        std::cout << "[ListAllocator] Checking merge: endCurrent (" 
                  << reinterpret_cast<void*>(endCurrent) 
                  << ") == nextBlock (" << current->next << ") : " 
                  << (can_merge ? "Yes" : "No") << "\n";

        if (can_merge) {
            std::cout << "[ListAllocator] Merging blocks at " << current 
                      << " and " << current->next << ".\n";
            // Сливаем current и current->next
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
            printFreeList();
        }
        else {
            current = current->next;
        }
    }
}

size_t ListAllocator::getLargestFreeBlock() const {
    size_t max_free_size = 0;
    Block* current = free_list;
    while (current) {
        if (current->is_free && current->size > max_free_size) {
            max_free_size = current->size;
        }
        current = current->next;
    }
    return max_free_size;
}

size_t ListAllocator::getTotalFreeMemory() const {
    size_t total_free_size = 0;
    Block* current = free_list;
    while (current) {
        if (current->is_free) {
            total_free_size += current->size;
        }
        current = current->next;
    }
    return total_free_size;
}
