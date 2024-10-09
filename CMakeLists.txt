cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

enable_testing()


add_compile_options(-Wall -Wextra -pedantic -Werror)


include(FindThreads)

add_subdirectory(lab1)

# add_subdirectory(tests)