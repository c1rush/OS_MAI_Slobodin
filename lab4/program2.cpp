#include <iostream>
#include <dlfcn.h>
#include <cstring>

using SquareFunc = float(*)(float, float);
using TranslationFunc = char*(*)(long);

int main() {
    std::cout << "Программа №2 (динамическая загрузка библиотек)\n";

    const char* pathToLib1 = std::getenv("PATH_TO_LIB1");
    if (!pathToLib1) {
        std::cerr << "Переменная окружения PATH_TO_LIB1 не установлена" << std::endl;
        return 1;
    }

    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена" << std::endl;
        return 1;
    }

    const char* lib_paths[] = {pathToLib1, pathToLib2};
    int current_lib = 0;
    void* handle = dlopen(lib_paths[current_lib], RTLD_LAZY);

    if (!handle) {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << "\n";
        return 1;
    }

    SquareFunc Square = reinterpret_cast<SquareFunc>(dlsym(handle, "Square"));
    TranslationFunc translation = reinterpret_cast<TranslationFunc>(dlsym(handle, "translation"));

    char* error;
    if ((error = dlerror()) != nullptr) {
        std::cerr << "Ошибка получения символа: " << error << "\n";
        dlclose(handle);
        return 1;
    }

    while (true) {
        std::cout << "\nТекущая библиотека: " << lib_paths[current_lib] << "\n";
        std::cout << "Введите команду:\n";
        std::cout << "0 - переключить реализацию\n";
        std::cout << "1 A B - вычислить площадь (прямоугольник или треугольник)\n";
        std::cout << "2 x - перевести число в другую систему счисления\n";
        std::cout << "3 - выйти\n";
        
        int command;
        std::cin >> command;

        if (command == 0) {
            dlclose(handle);
            current_lib = 1 - current_lib;
            handle = dlopen(lib_paths[current_lib], RTLD_LAZY);
            if (!handle) {
                std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << "\n";
                return 1;
            }
            Square = reinterpret_cast<SquareFunc>(dlsym(handle, "Square"));
            translation = reinterpret_cast<TranslationFunc>(dlsym(handle, "translation"));
        } else if (command == 1) {
            float A, B;
            std::cin >> A >> B;
            float result = Square(A, B);
            std::cout << "Площадь: " << result << "\n";
        } else if (command == 2) {
            long x;
            std::cin >> x;
            char* result = translation(x);
            std::cout << "Число в другой системе счисления: " << result << "\n";
        } else if (command == 3) {
            break;
        } else {
            std::cout << "Неверная команда\n";
        }
    }

    dlclose(handle);
    return 0;
}


// export PATH_TO_LIB1=$(pwd)/lab4/lib1.dylib
// export PATH_TO_LIB2=$(pwd)/lab4/lib2.dylib
// echo $PATH_TO_LIB1
// echo $PATH_TO_LIB2