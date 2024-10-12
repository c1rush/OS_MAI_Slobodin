set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(parent
    main.cpp
    src/parent.cpp
    src/utils.cpp
)

add_executable(child1
    src/child1.cpp
    src/utils.cpp
)

add_executable(child2
    src/child2.cpp
    src/utils.cpp
)

target_include_directories(parent PRIVATE include)
target_include_directories(child1 PRIVATE include)
target_include_directories(child2 PRIVATE include)