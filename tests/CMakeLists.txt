include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


# Тесты для 1 лр
add_executable(lab1_test lab1_test.cpp ../lab1/src/parent.cpp ../lab1/include/parent.h
        ../lab1/src/utils.cpp ../lab1/include/utils.h)

target_link_libraries(lab1_test PRIVATE GTest::GTest)
target_include_directories(lab1_test PRIVATE ../lab1/include)

add_test(lab1_test lab1_test)


# Тесты для 2 лр
add_executable(lab2_test lab2_test.cpp ../lab2/src/summing.cpp ../lab2/include/summing.h)

target_link_libraries(lab2_test PRIVATE GTest::GTest)
target_include_directories(lab2_test PRIVATE ../lab2/include)

add_test(lab2_test lab2_test)


# Тесты для 3 лр
add_executable(lab3_test lab3_test.cpp ../lab3/src/parent.cpp ../lab3/include/parent.h
        ../lab3/src/utils.cpp ../lab3/include/utils.h)

target_link_libraries(lab3_test PRIVATE GTest::GTest)
target_include_directories(lab3_test PRIVATE ../lab3/include)

add_test(lab3_test lab3_test)


# Тесты для лр 4

add_executable(lab4_test lab4_test.cpp)

target_link_libraries(lab4_test PRIVATE GTest::GTest)
target_include_directories(lab4_test PRIVATE ../lab4/include)
target_link_libraries(lab4_test PRIVATE lib1 m dl)

add_test(lab4_test lab4_test)


# Тесты для лр 5-7
add_executable(lab567_test lab567_test.cpp ../lab5-7/src/manage_zmq.cpp ../lab5-7/src/tree.cpp)
target_link_libraries(lab567_test PRIVATE GTest::GTest)
target_include_directories(lab567_test PRIVATE ../lab5-7/include)

target_link_libraries(lab567_test PRIVATE zmq)

add_test(lab567_test lab567_test)