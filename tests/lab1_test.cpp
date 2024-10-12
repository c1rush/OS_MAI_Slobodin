#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <parent.h>
#include <cstdio>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

// Класс для временного файла
class TempFile {
public:
    TempFile(const std::string& filename) : filename_(filename) {
        std::ofstream file(filename_);
        if (!file) {
            throw std::runtime_error("Не удалось создать временный файл: " + filename_);
        }
    }

    const std::string& getFilename() const {
        return filename_;
    }

    ~TempFile() {
        std::remove(filename_.c_str());
    }

private:
    std::string filename_;
};

// Функция для чтения содержимого файла
std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Тест на проверку работы с несколькими строками
TEST(ParentProcessTest, CheckStringsInFiles) {
    TempFile filename1("file1.txt");
    TempFile filename2("file2.txt");

    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    // Входной поток с тестовыми строками
    std::istringstream input_stream("qwerty\n12345\nabc\n123\n");

    // Вызов родительского процесса
    ParentRoutine(pathToChild1, pathToChild2, input_stream);

    // Проверка содержимого файлов
    std::string content1 = readFileContent(filename1.getFilename());
    std::string content2 = readFileContent(filename2.getFilename());

    EXPECT_EQ(content1, "ytrewq\ncba\n");  // Ожидаемый результат для нечетных строк
    EXPECT_EQ(content2, "54321\n321\n");   // Ожидаемый результат для четных строк
}

// Тест на пустой ввод
TEST(ParentProcessTest, EmptyInput) {
    TempFile filename1("file1.txt");
    TempFile filename2("file2.txt");

    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    // Входной поток без строк
    std::istringstream input_stream("");

    ParentRoutine(pathToChild1, pathToChild2, input_stream);

    std::string content1 = readFileContent(filename1.getFilename());
    std::string content2 = readFileContent(filename2.getFilename());

    // Проверка, что файлы пустые
    EXPECT_EQ(content1, "");
    EXPECT_EQ(content2, "");
}
