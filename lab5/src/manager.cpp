#include "zmq.hpp"
#include <string>
#include <iostream>
#include <functional>
#include <map>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

#include "base.hpp"

namespace lab5 {

class Manager {
private:
    zmq::context_t context;
    std::vector<NodeId> childs;
    std::map<NodeId, zmq::socket_t> childSockets;
public:
    Manager() {
        // Pass...
    }

    void AddChild(NodeId id, const std::string& pathToComputer) {
        if (std::count(childs.begin(), childs.end(), id) != 0) {
            return;
        }
        StartComputer(id, BASE_PORT + id, pathToComputer);
        childs.push_back(id);
        childSockets[id] = zmq::socket_t(context, zmq::socket_type::req);
        childSockets[id].bind("tcp://localhost:" + std::to_string(BASE_PORT + id));
    }

    int64_t makeChildSum(NodeId id, const std::vector<int64_t> &data) {
        if (std::count(childs.begin(), childs.end(), id) != 0) {
            throw std::logic_error("There's no child with such id!");
        }
        
    }
};

}

int main() {


    return 0;
}
