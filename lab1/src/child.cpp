#include "utils.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Necessary arguments were not provided\n");
        exit(EXIT_FAILURE);
    }

    // Открываем файл для записи
    FILE* out = fopen(argv[1], "w");
    if (!out) {
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }

    std::string input;
    while ((input = ReadString(stdin)).length() != 0) { // ТУТ ПОПРАВИТЬ!
        // Инвертируем строку
        std::reverse(input.begin(), input.end());
        // Пишем в файл
        fprintf(out, "%s\n", input.c_str()); 
    }

    fclose(out);
    return 0;
}