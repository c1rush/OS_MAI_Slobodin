#include "utils.h"
#include <unistd.h>
#include <cstring>

void CreatePipe(int pipeFd[2]) {
    if (pipe(pipeFd) == -1) {
        std::perror("Couldn't create pipe.");
        exit(EXIT_FAILURE);
    }
}

std::string ReadString(std::istream& stream) {
    std::string input;
    std::getline(stream, input);
    return input;
}

void Exec(const char * pathToChild, const std::string& exeFileName, const std::string& fileName) {
    execl(pathToChild, exeFileName.c_str(), fileName.c_str(), NULL);
    perror("exec failed");
    exit(EXIT_FAILURE);    
}