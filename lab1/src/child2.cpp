#include "utils.h"
#include <unistd.h>
#include <iostream>
#include <cstdio>

int main(int argc, char** argv) {
    if (argc != 1) {
        std::cerr << "Дочерний процесс 2: Необходимо передать имя файла для записи как аргумент" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[0], "w");
    if (!file) {
        std::perror("Дочерний процесс 2: Не удалось открыть файл");
        return 1;
    }

    dup2(fileno(file), STDOUT_FILENO);

    ReadData([](const std::string& str) {
        std::string res = Modify(str);
        write(STDOUT_FILENO, res.c_str(), res.size());
    }, std::cin);

    close(STDOUT_FILENO);

    return 0;
}