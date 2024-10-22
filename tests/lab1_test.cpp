#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <parent.h>
#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <sstream>

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
        if (std::filesystem::exists(filename)) {  // Проверяем, существует ли файл
            std::remove(filename.c_str()); 
        }
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

    const char* textFile1Path = std::getenv("TEXTFILE1_PATH");
    const char* textFile2Path = std::getenv("TEXTFILE2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "Переменная окружения CHILD1_PATH не установлена";
    ASSERT_NE(pathToChild2, nullptr) << "Переменная окружения CHILD2_PATH не установлена";

    ASSERT_NE(textFile1Path, nullptr) << "Переменная окружения TEXTFILE1_PATH не установлена";
    ASSERT_NE(textFile2Path, nullptr) << "Переменная окружения TEXTFILE2_PATH не установлена";

    // Входной поток с тестовыми строками
    std::istringstream input_stream(std::string(textFile1Path) + "\n" + std::string(textFile2Path) + "\nabc\n123\n");

    // Вызов родительского процесса
    ParentRoutine(pathToChild1, pathToChild2, input_stream);

    // Проверка содержимого файлов
    std::string content1 = readFileContent(filename1.getFilename());
    std::string content2 = readFileContent(filename2.getFilename());

    EXPECT_EQ(content1, "cba\n");  // Ожидаемый результат для нечетных строк
    EXPECT_EQ(content2, "321\n");   // Ожидаемый результат для четных строк
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

// export TEXTFILE1_PATH=$(pwd)/file1.txt
// export TEXTFILE2_PATH=$(pwd)/file2.txt
// Проверить установку пути:
// echo $TEXTFILE1_PATH
// echo $TEXTFILE2_PATH