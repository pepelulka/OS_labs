#pragma once

#include <iostream>
#include <sstream>

#include "base.hpp"

#include "zmq.hpp"

namespace lab5 {

class TComputerNode {
private:

    std::string path;
    NodeId id;
    Port pushPort, pullPort, pushPortBottom = 0, pingPort;
    bool haveChild = false;

    zmq::context_t context;
    zmq::socket_t pushSocket, pullSocket, pushSocketBottom;

public:

    TComputerNode(const std::string& _path, NodeId _id, Port _pullPort, Port _pushPort);

    void CreateChild(NodeId id1, Port bPullPort, Port bPushPort);

    void Routine();

};

}
