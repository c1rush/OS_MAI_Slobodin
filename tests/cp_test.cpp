// tests/cp_test.cpp

#include <gtest/gtest.h>
#include "../cp/src/buddy_allocator.hpp"
#include "../cp/src/list_allocator.hpp"

// Настройки для тестов
constexpr size_t TEST_MIN_BLOCK_SIZE = 32;

// Фикстура для BuddyAllocator
class BuddyAllocatorTest : public ::testing::Test {
protected:
    void* memory;
    BuddyAllocator* buddyAlloc;

    void SetUp() override {
        memory = std::malloc(1024 * 1024); // 1 MB
        ASSERT_NE(memory, nullptr) << "Failed to allocate memory for BuddyAllocator.";
        buddyAlloc = new BuddyAllocator(memory, 1024 * 1024, TEST_MIN_BLOCK_SIZE);
    }

    void TearDown() override {
        delete buddyAlloc;
        std::free(memory);
    }
};

// Фикстура для ListAllocator
class ListAllocatorTest : public ::testing::Test {
protected:
    void* memory;
    ListAllocator* listAlloc;

    void SetUp() override {
        memory = std::malloc(1024 * 1024); // 1 MB
        ASSERT_NE(memory, nullptr) << "Failed to allocate memory for ListAllocator.";
        listAlloc = new ListAllocator(memory, 1024 * 1024);
    }

    void TearDown() override {
        delete listAlloc;
        std::free(memory);
    }
};

// Утилита для проверки корректности адреса
bool is_within_memory(void* ptr, void* memory_start, size_t total_size) {
    return ptr >= memory_start && ptr < static_cast<char*>(memory_start) + total_size;
}

// Тесты для BuddyAllocator

TEST_F(BuddyAllocatorTest, SingleAllocation) {
    size_t alloc_size = 100;
    void* ptr = buddyAlloc->allocate(alloc_size);
    EXPECT_NE(ptr, nullptr);
    EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
    buddyAlloc->deallocate(ptr);
}

TEST_F(BuddyAllocatorTest, MultipleAllocations) {
    std::vector<void*> ptrs;
    for (size_t i = 0; i < 10; ++i) {
        void* ptr = buddyAlloc->allocate(64 * (i + 1));
        EXPECT_NE(ptr, nullptr);
        EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
        ptrs.push_back(ptr);
    }
    for (auto ptr : ptrs) {
        buddyAlloc->deallocate(ptr);
    }
}

TEST_F(BuddyAllocatorTest, AllocateUntilExhaustion) {
    std::vector<void*> ptrs;
    size_t alloc_size = 2048; // 2 KB

    while (true) {
        void* ptr = buddyAlloc->allocate(alloc_size);
        if (!ptr) break;
        EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
        ptrs.push_back(ptr);
    }

    // Освобождаем всю память
    for (auto ptr : ptrs) {
        buddyAlloc->deallocate(ptr);
    }

    // После освобождения должна быть доступна вся память
    EXPECT_EQ(buddyAlloc->getLargestFreeBlock(), 1024 * 1024);
    EXPECT_EQ(buddyAlloc->getTotalFreeMemory(), 1024 * 1024);
}

TEST_F(BuddyAllocatorTest, ZeroAllocation) {
    void* ptr = buddyAlloc->allocate(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(BuddyAllocatorTest, MaxAllocation) {
    // Максимальное выделение с учётом хранения order, если применимо
    size_t max_size = 1024 * 1024 - sizeof(int);
    void* ptr = buddyAlloc->allocate(max_size);
    EXPECT_NE(ptr, nullptr);
    EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
    buddyAlloc->deallocate(ptr);
}

TEST_F(BuddyAllocatorTest, ReallocationAfterDeallocation) {
    void* ptr1 = buddyAlloc->allocate(512);
    void* ptr2 = buddyAlloc->allocate(512);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_TRUE(is_within_memory(ptr1, memory, 1024 * 1024));
    EXPECT_TRUE(is_within_memory(ptr2, memory, 1024 * 1024));

    buddyAlloc->deallocate(ptr1);
    buddyAlloc->deallocate(ptr2);

    // Повторное выделение
    void* ptr3 = buddyAlloc->allocate(512);
    EXPECT_NE(ptr3, nullptr);
    EXPECT_TRUE(is_within_memory(ptr3, memory, 1024 * 1024));
    buddyAlloc->deallocate(ptr3);
}

// Тесты для ListAllocator

TEST_F(ListAllocatorTest, SingleAllocation) {
    size_t alloc_size = 128;
    void* ptr = listAlloc->allocate(alloc_size);
    EXPECT_NE(ptr, nullptr);
    EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
    listAlloc->deallocate(ptr);
}

TEST_F(ListAllocatorTest, MultipleAllocations) {
    std::vector<void*> ptrs;
    for (size_t i = 0; i < 10; ++i) {
        void* ptr = listAlloc->allocate(128 * (i + 1));
        EXPECT_NE(ptr, nullptr);
        EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
        ptrs.push_back(ptr);
    }
    for (auto ptr : ptrs) {
        listAlloc->deallocate(ptr);
    }
}

TEST_F(ListAllocatorTest, AllocateUntilExhaustion) {
    std::vector<void*> ptrs;
    size_t alloc_size = 4096; // 4 KB

    while (true) {
        void* ptr = listAlloc->allocate(alloc_size);
        if (!ptr) break;
        EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
        ptrs.push_back(ptr);
    }

    // Освобождаем всю память
    for (auto ptr : ptrs) {
        listAlloc->deallocate(ptr);
    }

    // После освобождения должна быть доступна вся память
    EXPECT_EQ(listAlloc->getLargestFreeBlock(), 1024 * 1024);
    EXPECT_EQ(listAlloc->getTotalFreeMemory(), 1024 * 1024);
}

TEST_F(ListAllocatorTest, ZeroAllocation) {
    void* ptr = listAlloc->allocate(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(ListAllocatorTest, MaxAllocation) {
    size_t max_size = 1024 * 1024;
    void* ptr = listAlloc->allocate(max_size);
    EXPECT_NE(ptr, nullptr);
    EXPECT_TRUE(is_within_memory(ptr, memory, 1024 * 1024));
    listAlloc->deallocate(ptr);
}

TEST_F(ListAllocatorTest, ReallocationAfterDeallocation) {
    void* ptr1 = listAlloc->allocate(1024);
    void* ptr2 = listAlloc->allocate(1024);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_TRUE(is_within_memory(ptr1, memory, 1024 * 1024));
    EXPECT_TRUE(is_within_memory(ptr2, memory, 1024 * 1024));

    listAlloc->deallocate(ptr1);
    listAlloc->deallocate(ptr2);

    // Повторное выделение
    void* ptr3 = listAlloc->allocate(1024);
    EXPECT_NE(ptr3, nullptr);
    EXPECT_TRUE(is_within_memory(ptr3, memory, 1024 * 1024));
    listAlloc->deallocate(ptr3);
}
