#include "manager_base.hpp"

namespace lab5 {

TTask TTask::MakeCreate(NodeId id, int parent) {
    return TTask{TTaskType::CREATE, id, parent};
}

TTask TTask::MakeExec(NodeId id, const std::vector<int>& data) {
    return TTask{TTaskType::EXEC, id, 0, data};
}

TTask TTask::MakePing() {
    return TTask{TTaskType::PING};
}

TTask TTask::MakeResult(const std::string &result) {
    return TTask{TTaskType::RESULT, 0, 0, {}, result};
}

std::set<NodeId> TSink::GetPingingResultAndFinishPinging() {
    std::lock_guard l(mtx);
    if (!pingingMode) {
        throw std::logic_error("Something defenitely went wrong");
    }
    std::set<NodeId> result;
    while (!pingingResult.empty()) {
        result.insert(pingingResult.top());
        pingingResult.pop();
    }
    pingingMode = false;
    return result;
}

TSink::TSink(TTaskQueue& _tq, Port _port) : tq(_tq), context(2), inputSocket(context, zmq::socket_type::pull), port(_port) {
    inputSocket.bind("tcp://localhost:" + std::to_string(port));
}

void TSink::Routine() {
    while (true) {
        zmq::message_t msg;
        auto result = inputSocket.recv(msg, zmq::recv_flags::none);
        std::string content((char*)msg.data(), (char*)msg.data() + msg.size());
        // Pinging
        if (content.size() >= 1 && content[0] == 'p') {
            std::lock_guard l(mtx);
            if (pingingMode) {
                std::stringstream ss(content);
                std::string dummy;
                ss >> dummy;
                NodeId id1;
                while (ss >> id1) {
                    pingingResult.push(id1);
                }
            }
            continue;
        }
        //
        tq.push(TTask::MakeResult(content));
    }
}

void TFrontend::Handle(const std::string &line) {
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
    } else if (command == "pingall") {
        tq->push(TTask::MakePing());
    }
}

void TFrontend::PushResult(const std::string &msg) {
    std::lock_guard l(mtx);
    q.push(msg);
}

void TFrontend::Routine() {
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

Port TPortPool::get() {
    lastPort++;
    if (lastPort == UPPER_PORT) {
        throw std::logic_error("Don't have enough ports...");
    }
    return lastPort;
}

void TNodeStructure::AddNewChild(NodeId id) {
    if (nodes.find(id) != nodes.end()) {
        front.PushResult("Error: Already exists");
        return ;
    }
    nodes.insert(id);
    childSets[id].insert(id);
    childPorts[id] = TPortPool::get();
    childSockets[id] = zmq::socket_t(context, zmq::socket_type::push);
    childSockets[id].connect("tcp://localhost:" + std::to_string(childPorts[id]));
    int pid = CreateProcess(path, id, childPorts[id], sinkPort);
}

void TNodeStructure::SendCreate(NodeId id, int parent) {
    if (parent == -1) {
        AddNewChild(id);
        return ;
    }

    if (nodes.find(id) != nodes.end()) {
        front.PushResult("Error: Already exists");
        return ;
    }

    if (nodes.find(parent) == nodes.end()) {
        front.PushResult("Error: Parent not found");
        return ;
    }

    for (const auto& p : childSets) {
        if (p.second.find(parent) != p.second.end()) {
            childSets[p.first].insert(id);
            nodes.insert(id);
            Port pingPort = TPortPool::get();
            zmq::message_t msg("create " + std::to_string(id) + " " + std::to_string(parent) + " " + std::to_string(TPortPool::get()) + " " + std::to_string(pingPort));
            childSockets[p.first].send(msg, zmq::send_flags::none);
        }
    }
}

void TNodeStructure::SendExec(NodeId id, const std::vector<int>& a) {
    if (nodes.find(id) == nodes.end()) {
        front.PushResult("Error:" + std::to_string(id) + ": Not found");
        return ;
    }

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

void TSink::StartPinging() {
    std::lock_guard l(mtx);
    pingingMode = true;
}

void TNodeStructure::SendPing() {
    sink.StartPinging();
    for (auto& p : childSockets) {
        zmq::message_t msg("r");
        p.second.send(msg, zmq::send_flags::none);
    }
    std::this_thread::sleep_for(FIRST_PING_TIME);
    // Harvest
    {
        std::set<NodeId> result = sink.GetPingingResultAndFinishPinging();
        if (result.size() == nodes.size()) {
            front.PushResult("Ok: -1");
            sinkSocket.send(zmq::message_t("Ok: -1"), zmq::send_flags::none);
        } else {
            std::string content = "Ok: ";
            for (const auto& nId : nodes) {
                if (result.find(nId) == result.end()) {
                    content += std::to_string(nId) + " ";
                }
            }
            front.PushResult(content);
            sinkSocket.send(zmq::message_t(content), zmq::send_flags::none);
        }
    }
}

TMainManager::TMainManager(const std::string &path, TFrontend &front) : sinkPort(TPortPool::get()), sink(wq, sinkPort), ns(path, sinkPort, front, sink), frontend(front) {
    sinkPort = sink.port;
    ns.sinkPort = sink.port;
    frontend.tq = &wq;
}

void TMainManager::Routine() {
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
            case TTaskType::PING:
                ns.SendPing();
                break;
        }
    }
}

}
