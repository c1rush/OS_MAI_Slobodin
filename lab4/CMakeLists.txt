# set(CMAKE_CXX_STANDARD_LIBRARIES "-stdlib=libc++")


# Компиляция первой библиотеки (lib1)
add_library(lib1 SHARED src/lib1.cpp)
set_target_properties(lib1 PROPERTIES OUTPUT_NAME "lib1" PREFIX "")
target_include_directories(lib1 PRIVATE include)

# Компиляция второй библиотеки (lib2)
add_library(lib2 SHARED src/lib2.cpp)
set_target_properties(lib2 PROPERTIES OUTPUT_NAME "lib2" PREFIX "")
target_include_directories(lib2 PRIVATE include)

# Установка пути для библиотек
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

# Компиляция первой программы (program1)
add_executable(program1 program1.cpp)
target_include_directories(program1 PRIVATE include)
target_link_libraries(program1 PRIVATE lib1 m)

# Компиляция второй программы (program2)
add_executable(program2 program2.cpp)
target_include_directories(program2 PRIVATE include)
target_link_libraries(program2 PRIVATE lib2 dl m)

