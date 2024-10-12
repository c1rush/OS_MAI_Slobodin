#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <parent.h>
#include "../lab1/include/utils.h"
#include <cstdio>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

// Класс для временного файла
class TempFile {
public:
    TempFile(const std::string& fName) : filename(fName) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Не удалось создать временный файл: " + filename);
        }
    }

    const std::string& getFilename() const {
        return filename;
    }

    ~TempFile() {
        std::remove(filename.c_str());
    }

private:
    std::string filename;
};

std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(UtilsTest, ModifyStringTest){
    std::string str = "qwerty";
    std::string res = "ytrewq";
    ASSERT_EQ(res, Modify(str));
}


TEST(ParentProcessTest, CheckStringsInFiles) {
    TempFile filename1("file1.txt");
    TempFile filename2("file2.txt");

    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    // Входной поток с тестовыми строками
    std::istringstream input_stream("/Users/c1rush/OS_MAI_Slobodin/build/file1.txt\n/Users/c1rush/OS_MAI_Slobodin/build/file2.txt\nabc\n123\n");

    // Вызов родительского процесса
    ParentRoutine(pathToChild1, pathToChild2, input_stream);

    // Проверка содержимого файлов
    std::string content1 = readFileContent(filename1.getFilename());
    std::string content2 = readFileContent(filename2.getFilename());

    EXPECT_EQ(content1, "\ncba");  // Ожидаемый результат для нечетных строк
    EXPECT_EQ(content2, "\n321");   // Ожидаемый результат для четных строк
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}