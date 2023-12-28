#pragma once


#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <sstream>
#include <stack>

#include "base.hpp"
#include "wqueue.hpp"

#include "zmq.hpp"

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

    //
    bool pingingMode = false;
    std::stack<NodeId> pingingResult; // Contains result of pinging, vector of nodeId's that is availible
    std::mutex mtx; // pingingResult & pingingMode mutex
    //

    Port port;

public:

    friend class TMainManager;

    std::set<NodeId> GetPingingResultAndFinishPinging();
    void StartPinging();

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
    std::string path;
    std::set<NodeId> nodes;
    std::map<NodeId, std::set<NodeId>> childSets;
    std::map<NodeId, zmq::socket_t> childSockets;
    std::map<NodeId, Port> childPorts;


    Port sinkPort;

    TFrontend &front;
    TSink &sink;
    zmq::socket_t sinkSocket;

public:

    friend class TMainManager;

    TNodeStructure() = delete;
    TNodeStructure(const std::string &_path,
                   Port _sinkPort,
                   TFrontend &_front,
                   TSink &_sink) : context(1), path(_path), sinkPort(_sinkPort), front(_front), sink(_sink), sinkSocket(context, zmq::socket_type::push) {
        sinkSocket.connect("tcp://localhost:" + std::to_string(sinkPort));
    }

    void AddNewChild(NodeId id);

    void SendCreate(NodeId id, int parent);

    void SendExec(NodeId id, const std::vector<int>& a);

    void SendPing();

};

// TMainManager stuff:

class TMainManager {
private:

    TTaskQueue wq;
    Port sinkPort;
public:
    TSink sink;
private:
    TNodeStructure ns;
    TFrontend &frontend;
public:

    friend class TSink;
    friend class TNodeStructure;
    friend class TFrontend;

    TMainManager(const std::string &path, TFrontend &front);

    void Routine();

};

}