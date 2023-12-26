#include "job_exec.hpp"

#include <atomic>

namespace cp {

int TSystem::Exec(const std::string& path) {
    int pid = fork();
    if (pid == 0) {
        if (execl(path.c_str(), path.c_str(), nullptr) == -1) {
            std::cout << "Can't exec\n";
        }
    } else if (pid == -1) {
        throw std::logic_error("Can't fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
    return 0;
}

void LogStack::Push(const std::string &str) {
    {
        std::unique_lock l(mut);
        completed.push_back(str);
    }
    cv.notify_one();
}

void TBasicExecutor::Execute(const std::string &path, const std::string &name) {
    std::thread t(&TBasicExecutor::RawExecute, this, name, path, log);
    t.detach();
}

void TBasicExecutor::RawExecute(const std::string &name, const std::string &path, LogStack *log) {
    {
        std::unique_lock<std::mutex> lk(mpcountMutex);
        if (maxProcCount == procCount) {
            mpcountCondVar.wait(lk);
        }
        procCount++;
    }

    try {
        int status = TSystem::Exec(path);
        if (status != 0) {
            exit(EXIT_FAILURE);
        }
    } catch (...) {
        exit(EXIT_FAILURE);
    }

    {
        std::unique_lock<std::mutex> lk(mpcountMutex);
        procCount--;
    }

    mpcountCondVar.notify_one();

    log->Push(name); // Warn about task completed
}

bool TDagJobExecutor::Execute(TJobDag &dag) {
    target = dag.jobs.size();
    current = 0;
    if (target == 0) {
        return true;
    }

    std::set<std::string> executionQueue;
    for (const auto& p : dag.jobs) {
        executionQueue.insert(p.first);
    }
    std::map<std::string, int> countOfDeps;
    for (const auto& p : dag.dep) {
        countOfDeps[p.first] = p.second.size();
    }

    // First layer
    for (const auto& p : dag.jobs) {
        if (countOfDeps[p.first] == 0) {
            actuallyReadyToBeExecuted.insert(p.first);
            executionQueue.erase(p.first);
        }
    }

    // Mutexes handling
    std::map<std::string, std::atomic<bool> > mutexState; // Owned - true, not owned - false
    for (const auto& i : dag.mutexes) {
        mutexState[i] = false;
    }

    // Execution
    procCount = 0;
    while (true) {
        if (current == target) {
            return true;
        } else {
            {
                std::unique_lock l2(mut);
                std::vector<std::string> toErase;
                for (const auto& job : actuallyReadyToBeExecuted) {
                    bool flag = true;
                    for (const auto& mutexOwn : dag.mutOwn[job]) {
                        if (mutexState[mutexOwn]) {
                            flag = false;
                            break;
                        }
                    }
                    if (!flag) {
                        continue;
                    }
                    for (const auto& mutexOwn : dag.mutOwn[job]) {
                        mutexState[mutexOwn] = true;
                    }
                    toErase.push_back(job);
                    ex.Execute(dag.jobs[job].path, job);

                }
                for (const auto& job : toErase) {
                    actuallyReadyToBeExecuted.erase(job);
                }
            }
            std::vector<std::string> completed;
            {
                std::unique_lock l(log.mut);
                if (log.wasRead == log.completed.size()) {
                    log.cv.wait(l);
                }
                for (size_t i = log.wasRead; i < log.completed.size(); i++) {
                    completed.push_back(log.completed[i]);
                    current++;
                }
                log.wasRead = log.completed.size();
            }
            for (const auto& job : completed) {
                for (const auto& depend : dag.rdep[job]) {
                    countOfDeps[depend]--;
                    if (countOfDeps[depend] == 0) {
                        actuallyReadyToBeExecuted.insert(depend);
                        executionQueue.erase(depend);
                    }
                }
                for (const auto& mtx : dag.mutOwn[job]) {
                    mutexState[mtx] = false;
                }
            }

        }
    }
}



}
