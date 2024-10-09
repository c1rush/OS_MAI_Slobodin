#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>

enum PipeEnd {
    READ_END,
    WRITE_END
};

void CreatePipe(int pipeFd[2]);
std::string ReadString(std::istream& stream);

#endif
