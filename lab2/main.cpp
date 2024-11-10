#include <iostream>  
#include <cstdlib> 
#include <chrono>    
#include <getopt.h> 
#include "summing.h" 

int main(int argc, char* argv[]){
    // Макс потоки по умолчанию
    int max_threads = 4;

    // Считываем флаги (макс потоки)
    int opt;
    while((opt = getopt(argc, argv, "t:")) != -1){
        switch(opt){
            case 't':
                max_threads = std::atoi(optarg);
                if(max_threads <= 0){
                    std::cerr << "Неверное количество потоков: " << optarg << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            default:
                std::cerr << "Использование: " << argv[0] << " -t <макс_потоки>" << std::endl;
                return EXIT_FAILURE;
        }
    }

    int K, N;
    std::cout << "Введите количество массивов (K): ";
    if(!(std::cin >> K) || K <= 0){
        std::cerr << "Неверное количество массивов." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Введите длину каждого массива (N): ";
    if(!(std::cin >> N) || N <= 0){
        std::cerr << "Неверная длина массивов." << std::endl;
        return EXIT_FAILURE;
    }

    int** arrays = create_arrays(K, N, 1);
    if(arrays == nullptr){
        std::cerr << "Не удалось выделить память для массивов." << std::endl;
        return EXIT_FAILURE;
    }

    int* result = create_result(N);
    if(result == nullptr){
        std::cerr << "Не удалось выделить память для результирующего массива." << std::endl;
        free_arrays(arrays, K);
        return EXIT_FAILURE;
    }

    int max_active_threads = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    if(!sum_arrays(K, N, arrays, result, max_threads, max_active_threads)){
        std::cerr << "Произошла ошибка при суммировании массивов." << std::endl;
        free_arrays(arrays, K);
        free_result(result);
        return EXIT_FAILURE;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end_time - start_time;

    // Отображение результата, если N <= 20
    if(N <= 20){
        std::cout << "Результирующий массив:" << std::endl;
        for(int i = 0; i < N; ++i){
            std::cout << result[i] << " ";
        }
        std::cout << std::endl;
    }
    else{
        // Вывод общей суммы всех элементов результирующего массива
        long long total_sum = 0;
        for(int i = 0; i < N; ++i){
            total_sum += result[i];
        }
        std::cout << "Общая сумма всех элементов результирующего массива: " << total_sum << std::endl;
    }

    std::cout << "Суммирование завершено." << std::endl;
    std::cout << "Максимальное количество одновременно работающих потоков: " << max_active_threads << std::endl;
    std::cout << "Время суммирования: " << duration.count() << " мс" << std::endl;

    free_arrays(arrays, K);
    free_result(result);

    return EXIT_SUCCESS;
}