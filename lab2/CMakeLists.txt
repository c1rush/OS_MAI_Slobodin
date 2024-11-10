
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_library(summing STATIC src/summing.cpp)
target_include_directories(summing PUBLIC include)

target_link_libraries(summing PRIVATE Threads::Threads)


add_executable(multithread main.cpp)
target_include_directories(multithread PUBLIC include)

target_link_libraries(multithread PRIVATE summing Threads::Threads)