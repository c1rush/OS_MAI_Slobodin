#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cmath>
#include <functions.h>

TEST(SquareTest, Implementation1) {
    // Тестируем Square из lib1 (прямоугольник)
    float A = 10.0f;
    float B = 5.0f;
    float result = Square(A, B);
    float expected = A * B;  // Площадь прямоугольника
    EXPECT_NEAR(result, expected, 1e-5);
}

TEST(SquareTest, Implementation2) {
    // Тестируем Square из lib2 (прямоугольный треугольник)
    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена" << std::endl;
        return exit(1);
    }

    void* handle = dlopen(pathToLib2, RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    using SquareFunc = float(*)(float, float);
    SquareFunc SquareLib2 = reinterpret_cast<SquareFunc>(dlsym(handle, "Square"));
    ASSERT_NE(SquareLib2, nullptr);

    float A = 10.0f;
    float B = 5.0f;
    float result = SquareLib2(A, B);
    float expected = 0.5f * A * B;  // Площадь прямоугольного треугольника
    EXPECT_NEAR(result, expected, 1e-5);

    dlclose(handle);
}

TEST(TranslationTest, Implementation1) {
    // Тестируем translation из lib1 (перевод в двоичную систему)
    long x = 10;
    char* result = translation(x);
    const char* expected = "1010";  // Ожидаемый результат: 10 в двоичной системе
    EXPECT_STREQ(result, expected);
}

TEST(TranslationTest, Implementation2) {
    // Тестируем translation из lib2 (перевод в троичную систему)
    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена" << std::endl;
        return exit(1);
    }

    void* handle = dlopen(pathToLib2, RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    using TranslationFunc = char*(*)(long);
    TranslationFunc translationLib2 = reinterpret_cast<TranslationFunc>(dlsym(handle, "translation"));
    ASSERT_NE(translationLib2, nullptr);

    long x = 9;
    char* result = translationLib2(x);
    const char* expected = "100";  // Ожидаемый результат: 9 в троичной системе
    EXPECT_STREQ(result, expected);

    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
