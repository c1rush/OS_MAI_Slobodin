cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_compile_options(-Wall -Wextra -pedantic)

find_package(Threads REQUIRED)

# add_subdirectory(lab1)

# add_subdirectory(lab2)

# add_subdirectory(lab3)

# add_subdirectory(lab4)

# add_subdirectory(lab5-7)

add_subdirectory(cp)

# add_subdirectory(tests)