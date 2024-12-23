#include "../include/tree.hpp"
#include "../include/manage_zmq.hpp"
#include <fstream>
#include <signal.h>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>  

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Not enough arguments");
        exit(EXIT_FAILURE);
    }

    Node task(atoi(argv[1]), atoi(argv[2]));
    std::string programPath = getenv("PROGRAM_PATH");

    while (1) {
        std::string message;
        std::string command = "";
        if (auto msg = ReceiveMessage(&(task.parent)); msg.has_value()) {
            message = *msg;
        } else {
            // Если сообщение не получено, продолжить
            continue;
        }
        std::istringstream request(message);
        request >> command;

        if (command == "create") {
            int idChild;
            request >> idChild;
            std::string ans = task.Create(idChild, programPath);
            SendMessage(&task.parent, ans);
        } else if (command == "pid") {
            std::string ans = task.Pid();
            SendMessage(&task.parent, ans);
        } else if (command == "ping") {
            int idChild;
            request >> idChild;
            std::string ans = task.Ping(idChild);
            SendMessage(&task.parent, ans);
        } else if (command == "send") {
            int id;
            request >> id;
            std::string str;
            getline(request, str);
            if (!str.empty() && str[0] == ' ') {
                str.erase(0, 1); // Удаление ведущего пробела
            }
            std::string ans;
            ans = task.Send(str, id);
            SendMessage(&task.parent, ans);
        } else if (command == "exec") {
            int targetId;
            request >> targetId;

            // Чтение остальной части сообщения
            std::string execArgs;
            if (std::getline(request, execArgs)) {
                // Удаление ведущего '|', если есть
                if (!execArgs.empty() && execArgs[0] == '|') {
                    execArgs.erase(0, 1);
                }

                // Разделение на text и pattern по '|'
                size_t sep = execArgs.find('|');
                if (sep != std::string::npos) {
                    std::string text = execArgs.substr(0, sep);
                    std::string pattern = execArgs.substr(sep + 1);

                    // Удаление лишних пробелов
                    text = trim(text);
                    pattern = trim(pattern);

                    // Ограничение длины строк
                    if (text.length() > 108 || pattern.length() > 108) {
                        SendMessage(&task.parent, "Error: Strings exceed maximum length of 108 characters");
                        continue;
                    }

                    // Логирование полученных данных (для отладки)
                    // std::cout << "Received exec command: ID=" << targetId
                    //           << ", Text='" << text << "', Pattern='" << pattern << "'" << std::endl;

                    // Выполнение поиска подстроки
                    std::vector<int> positions;
                    size_t pos = text.find(pattern, 0);
                    while(pos != std::string::npos) {
                        positions.push_back(static_cast<int>(pos));
                        pos = text.find(pattern, pos + 1);
                    }

                    // Формирование ответа
                    std::ostringstream response;
                    response << "Ok:" << targetId << ":";
                    if (!positions.empty()) {
                        for (size_t i = 0; i < positions.size(); ++i) {
                            response << positions[i];
                            if (i != positions.size() - 1) {
                                response << ";";
                            }
                        }
                    } else {
                        response << "-1"; 
                    }

                    // Логирование ответа (для отладки)
                    // std::cout << "Sending response: " << response.str() << std::endl;

                    SendMessage(&task.parent, response.str());
                } else {
                    SendMessage(&task.parent, "Error: Invalid exec command format");
                }
            } else {
                SendMessage(&task.parent, "Error: Invalid exec command format");
            }
        } else if (command == "kill") {
            std::string ans = task.Kill();
            ans = std::to_string(task.id) + " " + ans;
            SendMessage(&task.parent, ans);
            Disconnect(&task.parent, task.parentPort);
            task.parent.close();
            break;
        }
    }

    return 0;
}