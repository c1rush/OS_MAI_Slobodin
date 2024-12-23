#include "parent.h"
#include <iostream>

int main() {
    const char* child1Path = std::getenv("CHILD1_PATH");
    const char* child2Path = std::getenv("CHILD2_PATH");

    if (child1Path == nullptr) {
        std::cerr << "Error: Environment variable CHILD1_PATH is not set." << std::endl;
        return 1;
    }
    
    if (child2Path == nullptr) {
        std::cerr << "Error: Environment variable CHILD2_PATH is not set." << std::endl;
        return 1;
    }

    ParentRoutine(child1Path, child2Path, std::cin);

    return 0;
}

// Установить путь к исполняемым файлам(если делаем из папки build):
// export CHILD1_PATH=$(pwd)/lab3/lr3child1
// export CHILD2_PATH=$(pwd)/lab3/lr3child2

// Проверить установку пути:
// echo $CHILD1_PATH
// echo $CHILD2_PATH