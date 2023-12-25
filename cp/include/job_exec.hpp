#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>

#include <unistd.h>
#include <sys/wait.h>

#include "job_dag.hpp"

namespace cp {

class TSystem {
public:
    static int Exec(const std::string& path) {
        int pid = fork();
        if (pid == 0) {
            execv(path.c_str(), NULL);
        } else if (pid == -1) {
            throw std::logic_error("Can't fork");
        } else {
            int status;
            waitpid(pid, &status, 0);
            return status;
        }
    }
};

struct LogStack {
    // Stack of completed jobs
    std::vector<std::string> completed;
    size_t wasRead = 0;

    std::mutex mut;
    std::condition_variable cv;

    void push(const std::string &str) {
        std::unique_lock l(mut);
        completed.push_back(str);
        l.unlock();
        cv.notify_one();
    }

    // // return wasRead + 1 index when new info will appear
    // int read() {
    //     std::unique_lock l(mut);
    //     if (wasRead == completed.size()) {
    //         cv.wait(l);
    //     }
    //     return wasRead + 1;
    // }
};

class TBasicExecutor {
private:
    const static size_t STANDART_MAX_PROC_COUNT = 4;

    size_t maxProcCount;
    LogStack * log;

    size_t procCount = 0;
    std::mutex mpcountMutex;
    std::condition_variable mpcountCondVar;

    void RawExecute(const std::string &name, const std::string &path, LogStack *log) {
        std::unique_lock<std::mutex> lk1(mpcountMutex);
        if (maxProcCount == procCount) {
            mpcountCondVar.wait(lk1);
        }
        procCount++;
        lk1.unlock();

        int status = TSystem::Exec(path);
        if (status != 0) {
            exit(EXIT_FAILURE);
        }

        std::unique_lock<std::mutex> lk2(mpcountMutex);
        procCount--;
        lk2.unlock();
        mpcountCondVar.notify_one();

        log->push(name); // Warn about task completed
    }

public:

    TBasicExecutor(LogStack *_log) : maxProcCount(STANDART_MAX_PROC_COUNT), log(_log) { }
    TBasicExecutor(size_t mpCount, LogStack *_log) : maxProcCount(mpCount), log(_log) { }

    void Execute(const std::string &path, const std::string &name) {
        std::thread t(&TBasicExecutor::RawExecute, this, name, path, log);
        t.detach();
    }

};

class TDagJobExecutor {
private:

    const static size_t STANDART_MAX_PROC_COUNT = 4;
    size_t maxProcCount;

    size_t target, current;
    // std::mutex cMut;
    // std::condition_variable cCv;

    size_t procCount;
    std::set<std::string> actuallyReadyToBeExecuted;
    std::mutex mut; // procCount and sets
    std::condition_variable cv;

    LogStack log;
    TBasicExecutor ex;

public:

    TDagJobExecutor() : maxProcCount(STANDART_MAX_PROC_COUNT), ex(maxProcCount, &log) { }
    TDagJobExecutor(size_t mpCount) : maxProcCount(mpCount), ex(maxProcCount, &log) { }

    bool Execute(TJobDag &dag) {
        target = dag.jobs.size();
        current = 0;

        if (target == 0) {
            return true;
        }

        std::set<std::string> executionQueue;
        for (auto p : dag.jobs) {
            executionQueue.insert(p.first);
        }
        std::map<std::string, int> countOfDeps;
        for (auto p : dag.dep) {
            countOfDeps[p.first] = p.second.size();
        }

        // First layer
        for (auto p : dag.jobs) {
            if (countOfDeps[p.first] == 0) {
                actuallyReadyToBeExecuted.insert(p.first);
                executionQueue.erase(p.first);
            }
        }

        // Execution
        procCount = 0;
        while (true) {
            // std::unique_lock l1(cMut);
            if (current == target) {
                // cMut.unlock();
                return true;
            } else {
                {
                    std::unique_lock l2(mut);
                    while (!actuallyReadyToBeExecuted.empty()) {
                        std::string name = *actuallyReadyToBeExecuted.begin();
                        actuallyReadyToBeExecuted.erase(actuallyReadyToBeExecuted.begin());
                        ex.Execute(dag.jobs[name].path, name);
                    }
                }
                std::vector<std::string> completed;
                {
                    std::unique_lock l(log.mut);
                    if (log.wasRead == log.completed.size()) {
                        log.cv.wait(l);
                    }
                    for (int i = log.wasRead; i < log.completed.size(); i++) {
                        completed.push_back(log.completed[i]);
                        current++;
                    }
                    log.wasRead = log.completed.size();
                }
                for (auto job : completed) {
                    for (auto depend : dag.rdep[job]) {
                        countOfDeps[depend]--;
                        if (countOfDeps[depend] == 0) {
                            actuallyReadyToBeExecuted.insert(depend);
                            executionQueue.erase(depend);
                        }
                    }
                }

            }
        }

    }

};

}
