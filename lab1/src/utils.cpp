#include "utils.h"
#include <unistd.h>
#include <cstring>
#include <cstdio>

void CreatePipe(int pipeFd[2]) {
    if (pipe(pipeFd) == -1) {
        std::perror("Couldn't create pipe.");
        exit(EXIT_FAILURE);
    }
}

void ReadData(const std::function<void(const std::string&)>& handler, std::basic_istream<char>& stream){
    std::string buff;

    while (std::getline(stream, buff)){
        if (buff.empty()){
            return;
        }
        handler(buff + '\n');
    }
}

std::string Modify(const std::string& str) {
    std::string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}