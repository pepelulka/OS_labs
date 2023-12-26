#pragma once

#include "zmq.hpp"

#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <sstream>

#include "base.hpp"
#include "wqueue.hpp"

namespace lab5 {

class TMainManager;

enum TTaskType {
    CREATE, // create node
    EXEC, // exec
    RESULT, // result from sink
    PING // stop
};

struct TTask {
    TTaskType type;
    NodeId id;
    // for create
    int parent;
    // for exec
    std::vector<int> data;
    // for result
    std::string result;

    static TTask MakeCreate(NodeId id, int parent);

    static TTask MakeExec(NodeId id, const std::vector<int>& data);

    static TTask MakeResult(const std::string &result);

    static TTask MakePing();

};

using TTaskQueue = TWQueue<TTask>;

class TSink {
private:

    TTaskQueue &tq;

    zmq::context_t context;
    zmq::socket_t inputSocket;

    Port port;

public:

    friend class TMainManager;

    TSink(TTaskQueue& _tq, Port _port);

    void Routine();

};

class TFrontend {
private:

    std::queue<std::string> q;
    std::mutex mtx;

    TTaskQueue * tq;

    void Handle(const std::string &line);

public:

    friend class TMainManager;

    void PushResult(const std::string &msg);

    void Routine();

};

class TPortPool {
private:

    inline static Port lastPort = LOWER_PORT - 1;

public:

    TPortPool() = delete;

    static Port get();

};

// Manage nodes (and can push into them)
class TNodeStructure {
private:

    zmq::context_t context;
    std::set<NodeId> nodes;
    std::map<NodeId, std::set<NodeId>> childSets;
    std::map<NodeId, zmq::socket_t> childSockets;
    std::map<NodeId, Port> childPorts;

    std::map<NodeId, zmq::socket_t> pingSockets;

    std::string path;

    Port sinkPort;

    TFrontend &front;

public:

    friend class TMainManager;

    TNodeStructure() = delete;
    TNodeStructure(const std::string &_path, Port _sinkPort, TFrontend &_front) : context(3), path(_path), sinkPort(_sinkPort), front(_front) { }

    void AddNewChild(NodeId id);

    void SendCreate(NodeId id, int parent);

    void SendExec(NodeId id, const std::vector<int>& a);

    void SendPing();

};

// TMainManager stuff:

class TMainManager {
private:

    TTaskQueue wq;
    TNodeStructure ns;
    TFrontend &frontend;
    Port sinkPort;

public:
    TSink sink;

    friend class TSink;
    friend class TNodeStructure;
    friend class TFrontend;

    TMainManager(const std::string &path, TFrontend &front);

    void Routine();

};

}