#include <cmath>
#include <algorithm>
#include "functions.h"

// Реализация 1: Площадь прямоугольного треугольника
extern "C" {
    float Square(float A, float B) {
        return 0.5f * A * B; // Площадь прямоугольного треугольника
    }

    // Реализация 2: Перевод в троичную систему
    char* translation(long x) {
        static char buffer[65];  // Достаточно для представления числа в 64 бита
        int i = 0;
        if (x == 0) {
            buffer[i++] = '0';
        } else {
            while (x > 0) {
                buffer[i++] = (x % 3) + '0';
                x /= 3;
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
