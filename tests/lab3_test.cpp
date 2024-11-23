#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <parent.h>
#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

// Класс для временного файла
class TempFile {
public:
    TempFile(std::string fName) : filename(std::move(fName)) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Не удалось создать временный файл: " + filename);
        }
    }

    const std::string& getFilename() const {
        return filename;
    }

    ~TempFile() {
        if (fs::exists(filename)) {
            std::remove(filename.c_str());
        }
    }

private:
    std::string filename;
};

// Утилита для чтения содержимого файла
std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Тестирование утилитарной функции Modify
TEST(UtilsTest, ModifyStringTest) {
    std::string str = "qwerty";
    std::string res = "ytrewq";
    ASSERT_EQ(res, Modify(str));
}

// Проверка корректности взаимодействия через отображаемые файлы
TEST(ParentProcessTest, CheckStringsInFiles) {
    // Создаем временные файлы для вывода
    TempFile file1("file1.txt");
    TempFile file2("file2.txt");

    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    // Входной поток с тестовыми строками
    // Формат ввода: filename1\nfilename2\nстрока1\nстрока2\n...
    std::istringstream inputStream(file1.getFilename() + "\n" + file2.getFilename() + "\nabc\n123\n");

    // Вызов родительского процесса
    ParentRoutine(pathToChild1, pathToChild2, inputStream);

    // Проверка содержимого файлов
    std::string content1 = readFileContent(file1.getFilename());
    std::string content2 = readFileContent(file2.getFilename());

    EXPECT_EQ(content1, "cba\n");  // Ожидаемый результат для нечетных строк
    EXPECT_EQ(content2, "321\n");  // Ожидаемый результат для четных строк
}

// Проверка на пустой ввод
TEST(ParentProcessTest, EmptyInput) {
    // Создаем временные файлы для вывода
    TempFile file1("file1_empty.txt");
    TempFile file2("file2_empty.txt");

    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    // Входной поток без строк после имен файлов
    std::istringstream inputStream(file1.getFilename() + "\n" + file2.getFilename() + "\n");

    // Вызов родительского процесса
    ParentRoutine(pathToChild1, pathToChild2, inputStream);

    // Проверка, что файлы пустые
    std::string content1 = readFileContent(file1.getFilename());
    std::string content2 = readFileContent(file2.getFilename());

    EXPECT_EQ(content1, "");  // Ожидается, что файл останется пустым
    EXPECT_EQ(content2, "");  // Ожидается, что файл останется пустым
}

// Точка входа для тестов
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
