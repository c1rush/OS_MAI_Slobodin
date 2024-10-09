#include "parent.h"
#include "utils.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, FILE* stream) {
    char fileName1[256], fileName2[256];
    fscanf(stream, "%s\n", fileName1);
    fscanf(stream, "%s\n", fileName2);

    int pipe1[2], pipe2[2];
    CreatePipe(pipe1);
    CreatePipe(pipe2);

    int pid1 = fork();

    if (pid1 == 0) {
        close(pipe1[WRITE_END]);
        dup2(pipe1[READ_END], 0);
        execl(pathToChild1, "child1", fileName1, NULL);
        perror("exec failed"); // Обработка ошибок
        exit(EXIT_FAILURE);
    }

    int pid2 = fork();

    if (pid2 == 0) {
        close(pipe2[WRITE_END]);
        dup2(pipe2[READ_END], 0);
        execl(pathToChild2, "child2", fileName2, NULL);
        perror("exec failed"); // Обработка ошибок
        exit(EXIT_FAILURE);
    }

    close(pipe1[READ_END]);
    close(pipe2[READ_END]);

    std::string input;
    int lineNumber = 1; // Инициализируем номер строки

    while ((input = ReadString(std::cin)).length() != 0) {
        if (lineNumber % 2 == 1) {
            write(pipe1[WRITE_END], input.c_str(), input.length());
        } else {
            write(pipe2[WRITE_END], input.c_str(), input.length());
        }
        lineNumber++;
    }

    close(pipe1[WRITE_END]);
    close(pipe2[WRITE_END]);
    wait(NULL);
    wait(NULL);
}
