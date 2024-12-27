#include "../include/tree.hpp"

std::string Node::Ping(int _id) {
    std::string ans = "Ok:0"; 
    if (_id == id) {
        ans = "Ok:1";
        return ans;
    } else if (auto it = children.find(_id); it != children.end()) {
        std::string msg = "ping " + std::to_string(_id);
        SendMessage(it->second.get(), msg);          
        if (auto msg_resp = ReceiveMessage(children[_id].get()); msg_resp.has_value() && *msg_resp == "Ok:1") {
            ans = *msg_resp;
        }
        return ans;
    }
    return ans;
} 

std::string Node::Create(int idChild, const std::string& programPath) {
    std::string programName = programPath.substr(programPath.find_last_of("/") + 1);
    children[idChild] = std::make_unique<zmq::socket_t>(context, ZMQ_REQ);
    
    int newPort = Bind(children[idChild].get(), idChild);
    childrenPort[idChild] = newPort;
    int pid = fork();
    
    if (pid == 0) { // ребенок
        execl(programPath.c_str(), programName.c_str(), std::to_string(idChild).c_str(), std::to_string(newPort).c_str(), nullptr);
    } else { // родитель
        std::string pidChild = "Error: couldn't connect to child";
        children[idChild]->set(zmq::sockopt::sndtimeo, 3000);
        SendMessage(children[idChild].get(), "pid");
        if (auto msg = ReceiveMessage(children[idChild].get()); msg.has_value()) {
            pidChild = *msg;
        }
        return "Ok:" + pidChild;
    }
    return "";
}

std::string Node::Pid() {
    return std::to_string(getpid());
}

std::string Node::Send(const std::string& str, int id) {
    if (children.empty()) {
        return "Error: Not found";
    } else if (auto it = children.find(id); it != children.end()) {
        if (SendMessage(it->second.get(), str)) {
            std::string ans = "Error: Not found";
            if (auto msg = ReceiveMessage(children[id].get()); msg.has_value()) {
                ans = *msg;
                // Проверка, является ли команда 'kill'
                if (str.find("kill") == 0 && ans.find("Ok") == 0) {
                    // Удаление дочернего узла из списка
                    Unbind(children[it->first].get(), childrenPort[it->first]);
                    children[it->first]->close();
                    children.erase(it);
                    childrenPort.erase(id);
                    // std::cout << "Node " << id << " has been removed from children." << std::endl;
                }
            }
            return ans;
        }
    } else {
        std::string ans = "Error: Not found";
        for (auto& child : children) {
            std::string msg = "send " + std::to_string(id) + " " + str;
            if (SendMessage(child.second.get(), msg)) {
                if (auto msg_resp = ReceiveMessage(child.second.get()); msg_resp.has_value()) {
                    ans = *msg_resp;
                    // Если получили положительный ответ, прекращаем цикл
                    if (ans.find("Ok") == 0) {
                        break;
                    }
                }
            }
        }
        return ans;
    }
    return "Error: Not found";
}

std::string Node::Kill() {
    // Для отладки
    // std::cout << "Node " << id << " sending kill command to all children." << std::endl;
    // std::cout << "Children before kill: ";
    // for (const auto& [child_id, socket] : children) {
    //     std::cout << child_id << " ";
    // }
    // std::cout << std::endl;

    std::string ans;
    for (auto& child : children) {
        std::string msg = "kill";
        if (SendMessage(child.second.get(), msg)) {
            if (auto tmp = ReceiveMessage(child.second.get()); tmp.has_value()) {
                msg = *tmp;
                // Ответ от дочернего узла
                if (!ans.empty()) {
                    ans += " " + msg;
                } else {
                    ans = msg;
                }
            }
        }
        // Kill обработан в Send
    }
    return ans;
}

