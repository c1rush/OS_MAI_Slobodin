cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

enable_testing()


add_compile_options(-Wall -Wextra -pedantic -Werror)


find_package(Threads REQUIRED)

# add_subdirectory(lab1)

# add_subdirectory(lab2)

add_subdirectory(lab3)

add_subdirectory(tests)