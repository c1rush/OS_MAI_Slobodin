#include "parent.h"
#include <iostream>

int main() {

    ParentRoutine(std::getenv("CHILD1_PATH"), std::getenv("CHILD2_PATH"), std::cin);

    return 0;
}
// Установить путь к исполняемым файлам(если делаем из папки build):
// export CHILD1_PATH=$(pwd)/lab1/child1
// export CHILD2_PATH=$(pwd)/lab1/child2

// Проверить установку пути:
// echo $CHILD1_PATH
// echo $CHILD2_PATH