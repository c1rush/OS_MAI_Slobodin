#include "parent.h"
#include "utils.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, std::istream& stream) {
    std::string fileName1, fileName2;
    std::getline(stream, fileName1);
    std::getline(stream, fileName2);  

    int pipe1[2], pipe2[2];
    CreatePipe(pipe1);
    CreatePipe(pipe2);

    pid_t pid1 = fork();

    if (pid1 < 0) {
        std::perror("Failed to fork child1");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        close(pipe1[WRITE_END]);
        dup2(pipe1[READ_END], STDIN_FILENO);
        close(pipe1[READ_END]);
        Exec(pathToChild1, "child1", fileName1);
    }

    pid_t pid2 = fork();

    if (pid2 < 0) {
        std::perror("Failed to fork child2");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        close(pipe2[WRITE_END]);
        dup2(pipe2[READ_END], STDIN_FILENO);
        close(pipe2[READ_END]);
        Exec(pathToChild2, "child2", fileName2);
    } 

    // В родительском процессе закроем пайпы на чтение
    close(pipe1[READ_END]);
    close(pipe2[READ_END]);

    std::string input;
    size_t lineNumber{1};

    while (std::getline(stream, input)) {     
        if (input.empty()) continue;
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
