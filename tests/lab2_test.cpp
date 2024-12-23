#include <gtest/gtest.h>
#include "summing.h"
#include <chrono>
#include <cstdlib>

// Тест 1: Корректность работы однопоточного варианта
TEST(SummingTest, SingleThreadedCorrectness){
    int K = 3;
    int N = 10;
    int max_threads = 1;
    int max_active_threads = 0;

    int** arrays = create_arrays(K, N, 1);
    ASSERT_NE(arrays, nullptr) << "Не удалось выделить память для массивов";

    int* result = create_result(N);
    ASSERT_NE(result, nullptr) << "Не удалось выделить память для результирующего массива";

    bool success = sum_arrays(K, N, arrays, result, max_threads, max_active_threads);
    ASSERT_TRUE(success) << "Функция sum_arrays вернула false";

    // Проверка результатов
    for(int i = 0; i < N; ++i){
        EXPECT_EQ(result[i], K) << "Элемент " << i << " некорректен";
    }

    // Проверка максимального количества потоков
    EXPECT_LE(max_active_threads, max_threads) << "Максимальное количество активных потоков превышает заданное";

    free_arrays(arrays, K);
    free_result(result);
}

// Тест 2: Эквивалентность результатов однопоточного и многопоточного вариантов
TEST(SummingTest, EquivalenceSingleAndMultiThreaded){
    int K = 5;
    int N = 100;
    int max_threads_single = 1;
    int max_threads_multi = 4;
    int max_active_threads_single = 0;
    int max_active_threads_multi = 0;

    int** arrays = create_arrays(K, N, 2);
    ASSERT_NE(arrays, nullptr) << "Не удалось выделить память для массивов";

    int* result_single = create_result(N);
    ASSERT_NE(result_single, nullptr) << "Не удалось выделить память для результирующего массива";

    int* result_multi = create_result(N);
    ASSERT_NE(result_multi, nullptr) << "Не удалось выделить память для результирующего массива";

    // Однопоточный вариант
    bool success_single = sum_arrays(K, N, arrays, result_single, max_threads_single, max_active_threads_single);
    ASSERT_TRUE(success_single) << "Однопоточный sum_arrays вернул false";

    // Многопоточный вариант
    bool success_multi = sum_arrays(K, N, arrays, result_multi, max_threads_multi, max_active_threads_multi);
    ASSERT_TRUE(success_multi) << "Многопоточный sum_arrays вернул false";

    // Проверка эквивалентности результатов
    for(int i = 0; i < N; ++i){
        EXPECT_EQ(result_single[i], result_multi[i]) << "Элемент " << i << " отличается между вариантами";
    }

    // Проверка максимального количества потоков
    EXPECT_LE(max_active_threads_single, max_threads_single) << "Максимальное количество активных потоков в однопоточном варианте превышает заданное";
    EXPECT_LE(max_active_threads_multi, max_threads_multi) << "Максимальное количество активных потоков в многопоточном варианте превышает заданное";

    free_arrays(arrays, K);
    free_result(result_single);
    free_result(result_multi);
}

// Тест 3: Многопоточный вариант выполняется быстрее однопоточного
TEST(SummingTest, MultiThreadedFasterThanSingleThreaded){
    int max_threads_multi_ = 9;
    for(int i = 0; i < max_threads_multi_; ++i){
        int K = 99;
        int N = 1000000;
        int max_threads_single = 1;
        int max_threads_multi = i + 1;
        int max_active_threads_single = 0;
        int max_active_threads_multi = 0;
        int** arrays = create_arrays(K, N, 1);
        ASSERT_NE(arrays, nullptr) << "Не удалось выделить память для массивов";

        int* result_single = create_result(N);
        ASSERT_NE(result_single, nullptr) << "Не удалось выделить память для результирующего массива";

        int* result_multi = create_result(N);
        ASSERT_NE(result_multi, nullptr) << "Не удалось выделить память для результирующего массива";

        // Однопоточный вариант
        auto start_single = std::chrono::high_resolution_clock::now();
        bool success_single = sum_arrays(K, N, arrays, result_single, max_threads_single, max_active_threads_single);
        auto end_single = std::chrono::high_resolution_clock::now();
        ASSERT_TRUE(success_single) << "Однопоточный sum_arrays вернул false";
        auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(end_single - start_single).count();

        // Многопоточный вариант
        auto start_multi = std::chrono::high_resolution_clock::now();
        bool success_multi = sum_arrays(K, N, arrays, result_multi, max_threads_multi, max_active_threads_multi);
        auto end_multi = std::chrono::high_resolution_clock::now();
        ASSERT_TRUE(success_multi) << "Многопоточный sum_arrays вернул false";
        auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - start_multi).count();

        for(int i = 0; i < N; ++i){
            EXPECT_EQ(result_single[i], result_multi[i]) << "Элемент " << i << " отличается между вариантами";
        }


        EXPECT_LT(duration_multi, duration_single) << "Многопоточный вариант слишком медленный по сравнению с однопоточным";
        std::cout << "Время однопоточного варианта: " << duration_single << "ms" << std::endl;
        std::cout << "Время многопоточного варианта: " << duration_multi << "ms, Максимальное доступное число потоков: " << max_threads_multi << ", Максимально использовано потоков: " << max_active_threads_multi << "|" <<std::endl;
        std::cout << "------------------------------------------------------------------------------------------------------------------" << std::endl;
        // Проверка максимального количества потоков
        EXPECT_LE(max_active_threads_single, max_threads_single) << "Максимальное количество активных потоков в однопоточном варианте превышает заданное";
        EXPECT_LE(max_active_threads_multi, max_threads_multi) << "Максимальное количество активных потоков в многопоточном варианте превышает заданное";

        free_arrays(arrays, K);
        free_result(result_single);
        free_result(result_multi);        
    }

    for(int i = 0; i < max_threads_multi_; ++i){
        int K = 1000000;
        int N = 99;
        int max_threads_single = 1;
        int max_threads_multi = i + 1;
        int max_active_threads_single = 0;
        int max_active_threads_multi = 0;
        int** arrays = create_arrays(K, N, 1);
        ASSERT_NE(arrays, nullptr) << "Не удалось выделить память для массивов";

        int* result_single = create_result(N);
        ASSERT_NE(result_single, nullptr) << "Не удалось выделить память для результирующего массива";

        int* result_multi = create_result(N);
        ASSERT_NE(result_multi, nullptr) << "Не удалось выделить память для результирующего массива";

        // Однопоточный вариант
        auto start_single = std::chrono::high_resolution_clock::now();
        bool success_single = sum_arrays(K, N, arrays, result_single, max_threads_single, max_active_threads_single);
        auto end_single = std::chrono::high_resolution_clock::now();
        ASSERT_TRUE(success_single) << "Однопоточный sum_arrays вернул false";
        auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(end_single - start_single).count();

        // Многопоточный вариант
        auto start_multi = std::chrono::high_resolution_clock::now();
        bool success_multi = sum_arrays(K, N, arrays, result_multi, max_threads_multi, max_active_threads_multi);
        auto end_multi = std::chrono::high_resolution_clock::now();
        ASSERT_TRUE(success_multi) << "Многопоточный sum_arrays вернул false";
        auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - start_multi).count();

        for(int i = 0; i < N; ++i){
            EXPECT_EQ(result_single[i], result_multi[i]) << "Элемент " << i << " отличается между вариантами";
        }

        EXPECT_LT(duration_multi, duration_single) << "Многопоточный вариант слишком медленный по сравнению с однопоточным";
        std::cout << "Время однопоточного варианта: " << duration_single << "ms" << std::endl;
        std::cout << "Время многопоточного варианта: " << duration_multi << "ms, Максимальное доступное число потоков: " << max_threads_multi << ", Максимально использовано потоков: " << max_active_threads_multi << "|" <<std::endl;
        std::cout << "------------------------------------------------------------------------------------------------------------------" << std::endl;
        // Проверка максимального количества потоков
        EXPECT_LE(max_active_threads_single, max_threads_single) << "Максимальное количество активных потоков в однопоточном варианте превышает заданное";
        EXPECT_LE(max_active_threads_multi, max_threads_multi) << "Максимальное количество активных потоков в многопоточном варианте превышает заданное";

        free_arrays(arrays, K);
        free_result(result_single);
        free_result(result_multi);        
    }
}

// Дополнительный Тест 4: Некорректные входные данные
TEST(SummingTest, InvalidInput){
    int K = 0;
    int N = -10;
    int max_threads = 2;
    int max_active_threads = 0;

    int** arrays = nullptr;
    int* result = nullptr;

    bool success = sum_arrays(K, N, arrays, result, max_threads, max_active_threads);
    EXPECT_FALSE(success) << "sum_arrays должна возвращать false при некорректных K и N";
}

// Дополнительный Тест 5: Тест с одним массивом
TEST(SummingTest, SingleArray){
    int K = 1;
    int N = 10;
    int max_threads = 1;
    int max_active_threads = 0;

    int** arrays = create_arrays(K, N, 1);
    ASSERT_NE(arrays, nullptr) << "Не удалось выделить память для массивов";

    int* result = create_result(N);
    ASSERT_NE(result, nullptr) << "Не удалось выделить память для результирующего массива";

    bool success = sum_arrays(K, N, arrays, result, max_threads, max_active_threads);
    ASSERT_TRUE(success) << "Функция sum_arrays вернула false";

    for(int i = 0; i < N; ++i){
        EXPECT_EQ(result[i], 1) << "Элемент " << i << " некорректен";
    }

    EXPECT_LE(max_active_threads, max_threads) << "Максимальное количество активных потоков превышает заданное";

    free_arrays(arrays, K);
    free_result(result);
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
