#include <set>
#include <iostream>
#include <sstream>
#include <memory>
#include "../include/tree.hpp"
#include "../include/manage_zmq.hpp"

// export PROGRAM_PATH="/workspaces/OS_MAI_Slobodin/build/lab5-7/server"

int main() {
    std::set<int> Nodes;
    std::string programPath = getenv("PROGRAM_PATH");
    Node task(-1);
    Nodes.insert(-1);
    std::string command;
    while (std::cin >> command) {
        if (command == "create") {
            int idChild, idParent;
            std::cin >> idChild >> idParent;
            if (Nodes.find(idChild) != Nodes.end()) {
                std::cout << "Error: Already exists" << std::endl;
            } else if (Nodes.find(idParent) == Nodes.end()) {
                std::cout << "Error: Parent not found" << std::endl;
            } else if (idParent == task.id) { // from -1
                std::string ans = task.Create(idChild, programPath);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            } else { // from other node
                std::ostringstream strStream;
                strStream << "create " << idChild;
                std::string str = strStream.str();
                std::string ans = task.Send(str, idParent);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            }   
        } else if (command == "ping") {
            int idChild;
            std::cin >> idChild;
            if (Nodes.find(idChild) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            } else if (task.children.find(idChild) != task.children.end()) {
                std::string ans = task.Ping(idChild);
                std::cout << ans << std::endl;
            } else {
                std::ostringstream strStream;
                strStream << "ping " << idChild;
                std::string str = strStream.str();
                std::string ans = task.Send(str, idChild);
                if (ans == "Error: Not found") {
                    ans = "Ok:0"; // Убрали пробел
                }
                std::cout << ans << std::endl;
            }
        } else if (command == "exec") {
            int id;
            std::cin >> id;
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }

            std::string text, pattern;
            std::cin.ignore(); // Игнорируем оставшийся символ новой строки
            std::cout << "> "; // Запрос ввода text_string
            if (!std::getline(std::cin, text)) {
                std::cout << "Error: Failed to read text string" << std::endl;
                continue;
            }

            std::cout << "> "; // Запрос ввода pattern_string
            if (!std::getline(std::cin, pattern)) {
                std::cout << "Error: Failed to read pattern string" << std::endl;
                continue;
            }

            // Проверка длины строк
            if (text.length() > 108 || pattern.length() > 108) {
                std::cout << "Error: Strings exceed maximum length of 108 characters" << std::endl;
                continue;
            }

            // Формирование сообщения для отправки: "exec id|text|pattern"
            std::ostringstream msgStream;
            msgStream << "exec " << id << "|" << text << "|" << pattern;
            std::string execCommand = msgStream.str();

            // Отправка команды 'exec id|text|pattern'
            std::string ans = task.Send(execCommand, id);
            if (ans.empty()) {
                std::cout << "Error: No response from node" << std::endl;
                continue;
            }

            // Вывод ответа
            std::cout << ans << std::endl;
        } else if (command == "kill") {
            int id;
            std::cin >> id;
            std::string msg = "kill";
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            } else {
                std::string ans = task.Send(msg, id);
                if (ans != "Error: Not found") {
                    std::istringstream ids(ans);
                    int tmp;
                    while (ids >> tmp) {
                        Nodes.erase(tmp);
                    }
                    ans = "Ok";
                    if (task.children.find(id) != task.children.end()) {
                        Unbind(task.children[id].get(), task.childrenPort[id]);
                        task.children[id]->close();
                        task.children.erase(id);
                        task.childrenPort.erase(id);
                    }
                }
                std::cout << ans << std::endl;
            }
        } else if (command == "exit") {
            std::cout << "Executing kill on client..." << std::endl;
            task.Kill();
            // std::cout << "Kill executed, exiting program." << std::endl;
            return 0;
        }
    }
}