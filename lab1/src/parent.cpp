#include "parent.h"
#include "utils.h"
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

bool StartProcess(int * pipe, const std::string& childPath, std::string& filePath) {
    pid_t pid = fork();

    if (pid == -1){
        perror("Can't fork process");
        exit(-1);
    }

    if (pid == 0){
        close(pipe[WRITE_END]);
        dup2(pipe[READ_END], READ_END);

        if (execl(childPath.c_str(), const_cast<char *>(filePath.c_str()), NULL) == -1){
            std::cout << "Something went wrong when creating process " << childPath << std::endl;
        }
    }

    return pid == 0;
}

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, std::istream& input) {
    std::string filename1, filename2;

    std::cout << "Enter filename for 1 process: " << std::endl;
    std::getline(input, filename1);
    std::cout << "Enter filename for 2 process: " << std::endl;
    std::getline(input, filename2);

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

    close(pipe1[READ_END]);
    close(pipe2[READ_END]);

    wait(nullptr);
    wait(nullptr);
}