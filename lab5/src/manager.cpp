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
    STOP // stop
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

    static TTask MakeCreate(NodeId id, int parent) {
        return TTask{TTaskType::CREATE, id, parent};
    }

    static TTask MakeExec(NodeId id, const std::vector<int>& data) {
        return TTask{TTaskType::EXEC, id, 0, data};
    }

    static TTask MakeResult(const std::string &result) {
        return TTask{TTaskType::RESULT, 0, 0, {}, result};
    }

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

    TSink(TTaskQueue& _tq, Port _port) : tq(_tq), context(2), inputSocket(context, zmq::socket_type::pull), port(_port) {
        inputSocket.bind("tcp://localhost:" + std::to_string(port));
    }

    void Routine() {
        while (true) {
            zmq::message_t msg;
            auto result = inputSocket.recv(msg, zmq::recv_flags::none);
            std::string content((char*)msg.data(), (char*)msg.data() + msg.size());
            tq.push(TTask::MakeResult(content));
        }
    }

};

class TFrontend {
private:

    std::queue<std::string> q;
    std::mutex mtx;

    TTaskQueue * tq;

    void Handle(const std::string &line) {
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        if (command == "create") {
            size_t id;
            int parent;
            ss >> id >> parent;
            tq->push(TTask::MakeCreate(id, parent));
        } else if (command == "exec") {
            size_t id, n;
            ss >> id >> n;
            std::vector<int> a;
            for (size_t i = 0; i < n; i++) {
                int cur;
                ss >> cur;
                a.push_back(cur);
            }
            tq->push(TTask::MakeExec(id, a));
        }
    }

public:

    friend class TMainManager;

    void PushResult(const std::string &msg) {
        std::lock_guard l(mtx);
        q.push(msg);
    }

    void Routine() {
        std::string line;
        std::cout << "> ";
        while (std::getline(std::cin, line)) {
            {
                std::lock_guard l(mtx);
                while (!q.empty()) {
                    std::cout << q.front() << std::endl;
                    q.pop();
                }
            }
            Handle(line);
            std::cout << "> ";
        }

    }

};

class TPortPool {
private:

    inline static Port lastPort = LOWER_PORT - 1;

public:

    TPortPool() = delete;

    static Port get() {
        lastPort++;
        if (lastPort == UPPER_PORT) {
            throw std::logic_error("Don't have enough ports...");
        }
        return lastPort;
    }

};

// Manage nodes (and can push into them)
class TNodeStructure {
private:

    zmq::context_t context;
    std::set<NodeId> nodes;
    std::map<NodeId, std::set<NodeId>> childSets;
    std::map<NodeId, zmq::socket_t> childSockets;
    std::map<NodeId, Port> childPorts;

    std::string path;

    Port sinkPort;

    TFrontend &front;

    // TMainManager

public:

    friend class TMainManager;

    TNodeStructure() = delete;
    TNodeStructure(const std::string &_path, Port _sinkPort, TFrontend &_front) : context(3), path(_path), sinkPort(_sinkPort), front(_front) { }

    void AddNewChild(NodeId id) {
        if (childSets.find(id) != childSets.end()) {
            return;
        }
        childSets[id].insert(id);
        childPorts[id] = TPortPool::get();
        childSockets[id] = zmq::socket_t(context, zmq::socket_type::push);
        childSockets[id].bind("tcp://localhost:" + std::to_string(childPorts[id]));
        int pid = CreateProcess(path, id, childPorts[id], sinkPort);
        front.PushResult("OK: " + std::to_string(pid));
    }

    void SendCreate(NodeId id, int parent) {
        if (parent == -1) {
            AddNewChild(id);
            return ;
        }
        for (const auto& p : childSets) {
            if (p.second.find(parent) != p.second.end()) {
                childSets[p.first].insert(id);
                zmq::message_t msg("create " + std::to_string(id) + " " + std::to_string(parent) + " " + std::to_string(TPortPool::get()));
                childSockets[p.first].send(msg, zmq::send_flags::none);
            }
        }
    }

    void SendExec(NodeId id, const std::vector<int>& a) {
        for (const auto& p : childSets) {
            if (p.second.find(id) != p.second.end()) {
                childSets[p.first].insert(id);
                std::string content = "exec " + std::to_string(id) + " " + std::to_string(a.size()) + " ";
                for (const auto& i : a) {
                    content += std::to_string(i) + " ";
                }
                zmq::message_t msg(content);
                childSockets[p.first].send(msg, zmq::send_flags::none);
            }
        }
    }

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

    TMainManager(const std::string &path, TFrontend &front) : sinkPort(TPortPool::get()), ns(path, sinkPort, front), frontend(front), sink(wq, sinkPort) {
        sinkPort = sink.port;
        ns.sinkPort = sink.port;
        frontend.tq = &wq;
    }

    void Routine() {
        while (true) {
            TTask task = wq.pop();
            //
            switch (task.type) {
                case TTaskType::CREATE:
                    ns.SendCreate(task.id, task.parent);
                    break;
                case TTaskType::EXEC:
                    ns.SendExec(task.id, task.data);
                    break;
                case TTaskType::RESULT:
                    frontend.PushResult(task.result);
                    break;
            }
        }
    }

};

}

using namespace lab5;

int main() {
    std::string path = std::string(getenv("PATH_TO_COMP"));
    TFrontend front;
    TMainManager man(path, front);
    std::thread t1(&TFrontend::Routine, &front);
    std::thread t2(&TMainManager::Routine, &man);
    std::thread t3(&TSink::Routine, &man.sink);
    t1.join();
    t2.join();
    t3.join();
}
