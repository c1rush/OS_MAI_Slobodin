# cp/CMakeLists.txt

# Создаём библиотеку cp, которая будет содержать оба аллокатора
# add_library(cp
#     blockAllocator.hpp
#     freeListAllocator.hpp
# )

# # Указываем заголовочные файлы для установки
# target_include_directories(cp PUBLIC src)

# Создаём исполняемый файл
add_executable(cp_main main.cpp)

add_executable(cp_benchmark1 benchmark_one.cpp)

add_executable(cp_benchmark2 benchmark_two.cpp)

add_executable(cp_benchmark3 benchmark_three.cpp)

add_executable(cp_benchmark4 benchmark_four.cpp)

# Добавляем опции компиляции, если нужно
target_compile_options(cp_main PRIVATE -Wall -Wextra -pedantic)
