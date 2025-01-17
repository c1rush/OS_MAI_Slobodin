#include "parent.h"
#include "utils.h"
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

bool FileExists(const char* filename) {
    return access(filename, F_OK) != -1;  // F_OK проверяет существование файла
}

bool StartProcess(int * pipe, const char* childPath, const char* filePath) {
    pid_t pid = fork();

    if (pid == -1){
        perror("Can't fork process");
        exit(-1);
    }

    if (pid == 0){
        close(pipe[WRITE_END]);
        dup2(pipe[READ_END], READ_END);

        close(pipe[READ_END]);

        if (execl(childPath, filePath, nullptr) == -1){
            std::cout << "Something went wrong when creating process " << childPath << std::endl;
        }
    }

    return pid == 0;
}

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, std::istream& input) {
    char filename1[256];
    char filename2[256];

    std::cout << "Enter filename for 1 process: " << std::endl;
    input.getline(filename1, 256);
    std::cout << "Enter filename for 2 process: " << std::endl;
    input.getline(filename2, 256);

    // Проверяем существование файлов
    if (!FileExists(filename1)) {
        std::cerr << "Error: File " << filename1 << " does not exist." << std::endl;
        return;  // Завершаем выполнение, если файл не существует
    }

    if (!FileExists(filename2)) {
        std::cerr << "Error: File " << filename2 << " does not exist." << std::endl;
        return;  // Завершаем выполнение, если файл не существует
    }

    int pipe1[2], pipe2[2];
    CreatePipe(pipe1);
    CreatePipe(pipe2);

    std::cout << "FILE NAMES: "<< filename1 << " " << filename2 << std::endl;

    if (StartProcess(pipe2, pathToChild2, filename2)) return;
    if (StartProcess(pipe1, pathToChild1, filename1)) return;

    close(pipe1[READ_END]);
    close(pipe2[READ_END]);

    size_t lineNumber = 1;
    std::cout << "Enter strings to process: " << std::endl;

    ReadData([pipe1, pipe2, &lineNumber](const std::string& str) {
        if (lineNumber % 2 == 1) { write(pipe1[WRITE_END], str.c_str(), str.size()); } 
        else { write(pipe2[WRITE_END], str.c_str(), str.size()); }
        lineNumber++;
    }, input);

    write(pipe1[WRITE_END], "\n", 1); // Чтобы сработал выход в ReadData дочернего процесса
    write(pipe2[WRITE_END], "\n", 1);

    close(pipe1[WRITE_END]);
    close(pipe2[WRITE_END]);

    // close(pipe1[READ_END]);
    // close(pipe2[READ_END]);

    wait(nullptr);
    wait(nullptr);
}