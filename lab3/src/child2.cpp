#include "utils.h"
#include <atomic>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

#define SHM_SIZE 4096

std::atomic<bool> dataReady(false);
std::atomic<bool> terminateFlag(false);

// Обработчики сигналов
void dataSignalHandler(int signum) {
    (void)signum;
    dataReady.store(true, std::memory_order_relaxed);
}

void terminateSignalHandler(int signum) {
    (void)signum;
    terminateFlag.store(true, std::memory_order_relaxed);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Child2: Required arguments are shared memory name and output file name.\n");
        return 1;
    }

    const char* shmName = argv[1];
    const char* outputFileName = argv[2];

    // Открытие и маппинг разделяемой памяти
    int shmFd;
    char* shmPtr = CreateAndMapSharedMemory(shmName, shmFd, SHM_SIZE);

    // Регистрация обработчиков сигналов
    RegisterSignalHandler(SIGUSR2, dataSignalHandler);
    RegisterSignalHandler(SIGTERM, terminateSignalHandler);

    // Установка флага готовности
    shmPtr[0] = 1;

    // Открытие файла для вывода
    FILE* outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        perror("Failed to open output file");
        munmap(shmPtr, SHM_SIZE);
        close(shmFd);
        exit(EXIT_FAILURE);
    }

    // Основной цикл обработки
    while (!terminateFlag.load(std::memory_order_relaxed)) {
        if (dataReady.load(std::memory_order_relaxed)) {
            dataReady.store(false, std::memory_order_relaxed);

            std::string input(shmPtr + 2); // Пропуск флагов

            if (input.empty()) {
                break;
            }

            std::string modified = Modify(input);

            fprintf(outputFile, "%s", modified.c_str());

            // Установка флага завершения обработки
            shmPtr[1] = 1;
        } else {
            usleep(1000); // Небольшая задержка
        }
    }

    fclose(outputFile);
    munmap(shmPtr, SHM_SIZE);
    close(shmFd);

    return 0;
}
