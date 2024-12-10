#include <iostream>
#include "functions.h"

int main() {
    std::cout << "Программа №1 (статическая линковка библиотеки 1)\n";

    while (true) {
        std::cout << "\nВведите команду:\n";
        std::cout << "1 A B - вычислить площадь (прямоугольника)\n";
        std::cout << "2 x - перевести число в двоичную систему счисления\n";
        std::cout << "3 - выйти\n";
        
        int command;
        std::cin >> command;

        if (command == 1) {
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

    return 0;
}
