#include "../include/utils.h"
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <istream>

void ReadData(const std::function<void(const std::string&)>& handler, std::istream& stream) {
    std::string buff;
    while (std::getline(stream, buff)) {
        if (buff.empty()) return;
        handler(buff + '\n');
    }
}

std::string Modify(const std::string& str) {
    std::string result;
    if (!str.empty() && str.back() == '\n') {
        result = str.substr(0, str.size() - 1);
        std::reverse(result.begin(), result.end());
        result += '\n';
    } else {
        result = str;
        std::reverse(result.begin(), result.end());
    }
    return result;
}

// Маппинг разделяемой памяти
char* CreateAndMapSharedMemory(const char* shmName, int& shmFd, size_t size) {
    shmFd = shm_open(shmName, O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    char* shmPtr = static_cast<char*>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    if (shmPtr == MAP_FAILED) {
        perror("mmap failed");
        close(shmFd);
        exit(EXIT_FAILURE);
    }

    return shmPtr;
}

// Регистрация обработчиков сигналов
void RegisterSignalHandler(int signum, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(signum, &sa, nullptr) == -1) {
        perror("Failed to register signal handler");
        exit(EXIT_FAILURE);
    }
}