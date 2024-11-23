#include "parent.h"
#include "utils.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <csignal>
#include <cstdlib>

// Размер разделяемой памяти
#define SHM_SIZE 4096

// Функция для создания и маппинга разделяемой памяти
char* CreateAndMapSharedMemory(const std::string& shmName, int& shmFd) {
    shmFd = shm_open(shmName.c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
    if (shmFd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shmFd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        shm_unlink(shmName.c_str());
        exit(EXIT_FAILURE);
    }

    char* shmPtr = static_cast<char*>(mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    if (shmPtr == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink(shmName.c_str());
        exit(EXIT_FAILURE);
    }

    // Инициализация флагов готовности и завершения обработки
    shmPtr[0] = 0; // Флаг готовности
    shmPtr[1] = 0; // Флаг завершения обработки

    return shmPtr;
}

pid_t LaunchChildProcess(const char* childPath, const std::string& shmName, const char* outputFileName) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        if (execl(childPath, childPath, shmName.c_str(), outputFileName, nullptr) == -1) {
            perror("execl failed");
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, std::istream& input) {
    char filename1[256];
    char filename2[256];

    std::ostringstream oss1, oss2;
    static int counter = 0;
    oss1 << "/test_shm_" << getpid() << "_" << counter++ << "_1";
    oss2 << "/test_shm_" << getpid() << "_" << counter++ << "_2";
    std::string shmName1 = oss1.str();
    std::string shmName2 = oss2.str();

    // Игнорирование сигналов SIGUSR1 и SIGUSR2 в родительском процессе
    struct sigaction sa = {};
    sa.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGUSR2, &sa, nullptr);

    // Ввод имен файлов от пользователя
    std::cout << "Enter filename for 1 process: ";
    input.getline(filename1, 256);
    std::cout << "Enter filename for 2 process: ";
    input.getline(filename2, 256);

    // Создание и маппинг разделяемой памяти для child1
    int shmFd1;
    char* shmPtr1 = CreateAndMapSharedMemory(shmName1, shmFd1);

    // Создание и маппинг разделяемой памяти для child2
    int shmFd2;
    char* shmPtr2 = CreateAndMapSharedMemory(shmName2, shmFd2);

    // Запуск дочерних процессов
    pid_t child1_pid = LaunchChildProcess(pathToChild1, shmName1, filename1);
    pid_t child2_pid = LaunchChildProcess(pathToChild2, shmName2, filename2);

    // Ожидание готовности дочерних процессов
    while (shmPtr1[0] != 1 || shmPtr2[0] != 1) {
        usleep(1000); // 1 мс
    }

    size_t lineNumber = 1;
    ReadData([shmPtr1, shmPtr2, &lineNumber, child1_pid, child2_pid](const std::string& str) {
        if (lineNumber % 2 == 1) {
            // Запись данных в shmPtr1 + 2
            strncpy(shmPtr1 + 2, str.c_str(), SHM_SIZE - 3); // +2 для флагов
            shmPtr1[1] = 0; // Сброс флага завершения обработки
            shmPtr1[SHM_SIZE -1] = '\0'; // Обеспечение нулевого завершения строки
            kill(child1_pid, SIGUSR1);
            // Ожидание завершения обработки дочерним процессом
            while (shmPtr1[1] != 1) {
                usleep(1000);
            }
        } else {
            // Запись данных в shmPtr2 + 2
            strncpy(shmPtr2 + 2, str.c_str(), SHM_SIZE - 3); // +2 для флагов
            shmPtr2[1] = 0; // Сброс флага завершения обработки
            shmPtr2[SHM_SIZE -1] = '\0'; // Обеспечение нулевого завершения строки
            kill(child2_pid, SIGUSR2);
            // Ожидание завершения обработки дочерним процессом
            while (shmPtr2[1] != 1) {
                usleep(1000);
            }
        }
        lineNumber++;
    }, input);

    // Отправка сигналов завершения дочерним процессам
    kill(child1_pid, SIGTERM);
    kill(child2_pid, SIGTERM);

    // Ожидание завершения дочерних процессов
    waitpid(child1_pid, nullptr, 0);
    waitpid(child2_pid, nullptr, 0);

    // Очистка ресурсов
    munmap(shmPtr1, SHM_SIZE);
    munmap(shmPtr2, SHM_SIZE);
    close(shmFd1);
    close(shmFd2);
    shm_unlink(shmName1.c_str());
    shm_unlink(shmName2.c_str());
}
