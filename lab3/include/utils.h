#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <string>

// Для чтения и обработки данных
void ReadData(const std::function<void(const std::string&)>& handler, std::istream& stream);

// Модификация строки
std::string Modify(const std::string& str);

// Создание и маппинг разделяемой памяти
char* CreateAndMapSharedMemory(const char* shmName, int& shmFd, size_t size);

// Регистрация обработчиков сигналов
void RegisterSignalHandler(int signum, void (*handler)(int));

#endif
