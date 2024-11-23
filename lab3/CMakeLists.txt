set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(lr3parent
    main.cpp
    src/parent.cpp
    src/utils.cpp
)

add_executable(lr3child1
    src/child1.cpp
    src/utils.cpp
)

add_executable(lr3child2
    src/child2.cpp
    src/utils.cpp
)

target_include_directories(lr3parent PRIVATE include)
target_include_directories(lr3child1 PRIVATE include)
target_include_directories(lr3child2 PRIVATE include)