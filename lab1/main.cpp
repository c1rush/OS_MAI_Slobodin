#include "parent.h"
#include <iostream>

int main() {
    const char* pathToChild1 = "./child";  // Указываем путь к программе дочернего процесса 1
    const char* pathToChild2 = "./child";  // Указываем путь к программе дочернего процесса 2

    std::cout << "Введите строки:\n";
    ParentRoutine(pathToChild1, pathToChild2, std::cin);

    return 0;
}
