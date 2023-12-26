#include <iostream>
#include <sstream>

#include "base.hpp"

#include "zmq.hpp"

namespace lab5 {

class TComputerNode {
private:

    std::string path;
    NodeId id;
    Port pushPort, pullPort, pushPortBottom = 0;
    bool haveChild = false;

    zmq::context_t context;
    zmq::socket_t pushSocket, pullSocket, pushSocketBottom;

public:

    TComputerNode(const std::string& _path, NodeId _id, Port _pullPort, Port _pushPort) : id(_id), pullPort(_pullPort), pushPort(_pushPort), context(2), path(_path),
                                                                                          pullSocket(context, zmq::socket_type::pull),
                                                                                          pushSocket(context, zmq::socket_type::push),
                                                                                          pushSocketBottom(context, zmq::socket_type::push) {
        pushSocket.connect("tcp://localhost:" + std::to_string(pushPort));
        pullSocket.connect("tcp://localhost:" + std::to_string(pullPort));
    }

    void CreateChild(NodeId id1, Port bPullPort, Port bPushPort) {
        if (haveChild) {
            return ;
        }
        pushPortBottom = bPullPort;
        pushSocketBottom.bind("tcp://localhost:" + std::to_string(pushPortBottom));
        haveChild = true;
        int pid = CreateProcess(path, id1, bPullPort, bPushPort);
        std::cout << "I'm pushing on port " << pushPort << std::endl;
        zmq::message_t msg("OK: " + std::to_string(pid));
        pushSocket.send(msg, zmq::send_flags::none);
    }

    void Routine() {
        while (true) {
            zmq::message_t msg;
            std::cout << "listening start... port = " << pullPort << std::flush;
            pullSocket.recv(msg, zmq::recv_flags::none);
            std::cout << " ... listening end port = " << pullPort << std::endl;
            std::string content((char*)msg.data(), (char*)msg.data() + msg.size());
            std::cout << "Port " << pullPort << " got msg: " << content << std::endl;
            std::stringstream ss(content);
            std::string command;
            ss >> command;
            if (command == "create") {
                size_t id1, parent, port1;
                ss >> id1 >> parent >> port1;
                if (parent == id) {
                    CreateChild(id1, port1, pullPort);
                } else {
                    pushSocketBottom.send(msg, zmq::send_flags::none);
                }
            } else if (command == "exec") {
                size_t id1, n;
                ss >> id1;
                if (id != id1) {
                    std::cout << "forwarding to port " << pushPortBottom << std::endl;
                    pushSocketBottom.send(msg, zmq::send_flags::none);
                    std::cout << "pre-continue id = " << id << std::endl;
                    // continue;
                } else {
                    ss >> n;
                    int sum = 0;
                    for (size_t i = 0; i < n;i++) {
                        int cur;
                        ss >> cur;
                        sum += cur;
                    }
                    zmq::message_t msg("OK:" + std::to_string(id) + ": " + std::to_string(sum));
                    std::cout << "Pushing result onto " << pushPort << std::endl;
                    pushSocket.send(msg, zmq::send_flags::none);
                }
            } else {
                std::cout << "Forwarding message from bottom\n";
                pushSocket.send(msg, zmq::send_flags::none);
            }
        }
    }

};

}

using namespace lab5;

int main(int argc, char ** argv) {

    if (argc < 4) {
        std::cerr << "Missing arguments!\n";
        exit(EXIT_FAILURE);
    }
    NodeId id = std::atoi(argv[1]);
    Port pullPort = std::atoi(argv[2]), pushPort = std::atoi(argv[3]);

    TComputerNode node(std::string(getenv("PATH_TO_COMP")), id, pullPort, pushPort);
    node.Routine();
}