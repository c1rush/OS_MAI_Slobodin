#include <cmath>
#include <algorithm>
#include "functions.h"

// Реализация 1: Площадь прямоугольника
extern "C" {
    float Square(float A, float B) {
        return A * B; // Площадь прямоугольника
    }

    // Реализация 2: Перевод в двоичную систему
    char* translation(long x) {
        static char buffer[65];  // Достаточно для представления числа в 64 бита
        int i = 0;
        if (x == 0) {
            buffer[i++] = '0';
        } else {
            while (x > 0) {
                buffer[i++] = (x % 2) + '0';
                x /= 2;
            }
        }
        buffer[i] = '\0';

        // Переворачиваем строку
        for (int j = 0, k = i - 1; j < k; ++j, --k) {
            std::swap(buffer[j], buffer[k]);
        }

        return buffer;
    }
}
