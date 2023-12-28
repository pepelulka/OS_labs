#pragma once

#include <iostream>
#include <sstream>

#include "base.hpp"
#include "zmq.hpp"

namespace lab5 {

class TComputerNode {
private:

    NodeId id;
    Port pullPort = 0, pushPort = 0, pushPortBottom = 0;
    zmq::context_t context;
    std::string path;
    zmq::socket_t pullSocket, pushSocket, pushSocketBottom;
    bool haveChild = false;


public:

    TComputerNode(const std::string& _path, NodeId _id, Port _pullPort, Port _pushPort);

    void CreateChild(NodeId id1, Port bPullPort, Port bPushPort);

    void Routine();

};

}
