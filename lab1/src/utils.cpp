#include "utils.h"
#include <stdexcept>

void CreatePipe(int pipeFd[2]) {
    if (pipe(pipeFd) == -1) {
        throw std::runtime_error("Не удалось создать pipe ;(");
    }
}

std::string ReadString(std::istream& stream) {
    std::string input;
    std::getline(stream, input);
    return input;
}