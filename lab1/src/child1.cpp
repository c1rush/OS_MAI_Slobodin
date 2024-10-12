#include "utils.h"
#include <fstream>
#include <unistd.h>
#include <algorithm>

int main(const int argc, const char* argv[]) {
    if(argc != 2) {
        std::perror("Necessary arguments were not provided to first child process");
        exit(EXIT_FAILURE);
    }

    const char* fileName = argv[1];
    std::ofstream outFile(fileName, std::ios::app);

    if(!outFile.is_open()) {
        std::perror("Failed to open file for write");
        exit(EXIT_FAILURE);
    }

    std::string input;
    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        std::string input(buffer, bytesRead);

        std::reverse(input.begin(), input.end());
        outFile << input << '\n';
        outFile.flush();
    }

    outFile.close();
    return 0;
}