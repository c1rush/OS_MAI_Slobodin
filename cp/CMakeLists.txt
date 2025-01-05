cmake_minimum_required(VERSION 3.20)

project(cp_allocators LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем флаги для AddressSanitizer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

# Пусть исходники лежат в cp/src
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Собираем список cpp-файлов
set(SOURCES
    ${SRC_DIR}/buddy_allocator.cpp
    ${SRC_DIR}/list_allocator.cpp
    ${SRC_DIR}/main.cpp
)

add_executable(cp_run ${SOURCES})

# Включаем заголовочные пути (cp/src)
target_include_directories(cp_run PRIVATE ${SRC_DIR})
