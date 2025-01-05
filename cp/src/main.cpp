#include <iostream>
#include <cstdlib>
#include "buddy_allocator.hpp"
#include "list_allocator.hpp"

int main() {
    const size_t MEMORY_SIZE = 1024 * 1024; // 1 MB

    // Выделяем память для BuddyAllocator и ListAllocator
    void* memory_for_buddy = std::malloc(MEMORY_SIZE);
    void* memory_for_list  = std::malloc(MEMORY_SIZE);

    if (!memory_for_buddy || !memory_for_list) {
        std::cerr << "Failed to allocate test memory." << std::endl;
        return 1;
    }

    {
        // Создаём объекты BuddyAllocator и ListAllocator внутри блока
        BuddyAllocator buddyAlloc(memory_for_buddy, MEMORY_SIZE, 32);
        ListAllocator  listAlloc(memory_for_list, MEMORY_SIZE);

        // Простейший тест для BuddyAllocator
        std::cout << "=== BuddyAllocator Test ===" << std::endl;
        void* b1 = buddyAlloc.allocate(100);
        void* b2 = buddyAlloc.allocate(2000);
        void* b3 = buddyAlloc.allocate(50000);
        std::cout << "Allocated buddy blocks: " << b1 << ", " << b2 << ", " << b3 << std::endl;

        std::cout << "Largest free block: " << buddyAlloc.getLargestFreeBlock() << " bytes" << std::endl;
        std::cout << "Total free memory:  " << buddyAlloc.getTotalFreeMemory() << " bytes" << std::endl;

        buddyAlloc.deallocate(b2);
        std::cout << "After dealloc b2" << std::endl;
        std::cout << "Largest free block: " << buddyAlloc.getLargestFreeBlock() << " bytes" << std::endl;
        std::cout << "Total free memory:  " << buddyAlloc.getTotalFreeMemory() << " bytes" << std::endl;

        // Простейший тест для ListAllocator
        std::cout << "\n=== ListAllocator Test ===" << std::endl;
        void* l1 = listAlloc.allocate(128);
        void* l2 = listAlloc.allocate(2016);
        void* l3 = listAlloc.allocate(50016);
        std::cout << "Allocated list blocks: " << l1 << ", " << l2 << ", " << l3 << std::endl;

        std::cout << "Largest free block: " << listAlloc.getLargestFreeBlock() << " bytes" << std::endl;
        std::cout << "Total free memory:  " << listAlloc.getTotalFreeMemory() << " bytes" << std::endl;

        listAlloc.deallocate(l2);
        std::cout << "After dealloc l2" << std::endl;
        std::cout << "Largest free block: " << listAlloc.getLargestFreeBlock() << " bytes" << std::endl;
        std::cout << "Total free memory:  " << listAlloc.getTotalFreeMemory() << " bytes" << std::endl;
    } // Деструкторы вызываются здесь

    // Освобождаем память после вызова деструкторов
    std::free(memory_for_buddy);
    std::free(memory_for_list);

    return 0;
}
