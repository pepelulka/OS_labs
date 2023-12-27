#include "computer_base.hpp"

namespace lab5 {

TComputerNode::TComputerNode(const std::string& _path, NodeId _id, Port _pullPort, Port _pushPort) : id(_id), pullPort(_pullPort), pushPort(_pushPort), context(1), path(_path),
                                                                                                     pullSocket(context, zmq::socket_type::pull),
                                                                                                     pushSocket(context, zmq::socket_type::push),
                                                                                                     pushSocketBottom(context, zmq::socket_type::push) {
    pushSocket.connect("tcp://localhost:" + std::to_string(pushPort));
    pullSocket.bind("tcp://localhost:" + std::to_string(pullPort));
}

void TComputerNode::CreateChild(NodeId id1, Port bPullPort, Port bPushPort) {
    if (haveChild) {
        return ;
    }
    pushPortBottom = bPullPort;
    pushSocketBottom.connect("tcp://localhost:" + std::to_string(pushPortBottom));
    haveChild = true;
    int pid = CreateProcess(path, id1, bPullPort, bPushPort);
    zmq::message_t msg("OK: " + std::to_string(pid));
    pushSocket.send(msg, zmq::send_flags::none);
}

void TComputerNode::Routine() {
    while (true) {
        zmq::message_t msg;
        pullSocket.recv(msg, zmq::recv_flags::none);
        std::string content((char*)msg.data(), (char*)msg.data() + msg.size());
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
                pushSocketBottom.send(msg, zmq::send_flags::none);
                continue;
            } else {
                ss >> n;
                int sum = 0;
                for (size_t i = 0; i < n;i++) {
                    int cur;
                    ss >> cur;
                    sum += cur;
                }
                zmq::message_t msg("OK:" + std::to_string(id) + ": " + std::to_string(sum));
                auto ret = pushSocket.send(msg, zmq::send_flags::none);
            }
        } else {
            pushSocket.send(msg, zmq::send_flags::none);
        }
    }
}

}
