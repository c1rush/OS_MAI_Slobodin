#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <functional>

enum PipeEnd {
    READ_END,
    WRITE_END
};

void CreatePipe(int pipeFd[2]);

void ReadData(const std::function<void(const std::string&)>& handler, std::basic_istream<char>& stream);

std::string Modify(const std::string& str);

#endif
