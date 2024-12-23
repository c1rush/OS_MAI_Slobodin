#include "../include/manage_zmq.hpp"


int Bind(zmq::socket_t *socket, int id) {
    int port = 4040 + id;
    while(true) {
        std::string address = "tcp://127.0.0.1:"  + std::to_string(port);
        try{
            socket->bind(address);
            break;
        } catch(...) {
            port++;
        }    
    }
    return port;
}
void Unbind(zmq::socket_t *socket, int port) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(port);
    socket->unbind(address);
}

void Connect(zmq::socket_t *socket, int port) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(port);
    socket->connect(address);
}

void Disconnect(zmq::socket_t *socket, int port) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(port);
    socket->disconnect(address);
}

bool SendMessage(zmq::socket_t *socket, const std::string& msg) {
    try {
        zmq::message_t message(msg.size());
        memcpy(message.data(), msg.c_str(), msg.size());
        socket->send(message, zmq::send_flags::none);
        return true;
    } catch(const zmq::error_t& e) {
        std::cerr << "SendMessage error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::string> ReceiveMessage(zmq::socket_t* socket) {
    zmq::message_t message;
    try {
        auto result = socket->recv(message, zmq::recv_flags::none);
        if (result && *result > 0) {
            return std::string(static_cast<char*>(message.data()), message.size());
        }
    } catch(const zmq::error_t& e) {
        std::cerr << "ReceiveMessage warning: " << e.what() << std::endl;
    }
    return std::nullopt;
}

