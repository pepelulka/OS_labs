#include "job_exec.hpp"

namespace cp {

void TBasicExecutor::RawExecute(const std::string &name, const std::string &path, LogStack *log) {
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

bool TDagJobExecutor::Execute(TJobDag &dag) {
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

    // Mutexes handling
    std::map<std::string, bool> mutexState; // Owned - true, not owned - false
    for (auto i : dag.mutexes) {
        mutexState[i] = false;
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
                std::vector<std::string> toErase;
                for (auto job : actuallyReadyToBeExecuted) {
                    bool flag = true;
                    for (auto mutexOwn : dag.mutOwn[job]) {
                        if (mutexState[mutexOwn]) {
                            flag = false;
                            break;
                        }
                    }
                    if (!flag) {
                        continue;
                    }
                    for (auto mutexOwn : dag.mutOwn[job]) {
                        mutexState[mutexOwn] = true;
                    }
                    toErase.push_back(job);
                    ex.Execute(dag.jobs[job].path, job);

                }
                for (auto job : toErase) {
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
            for (auto job : completed) {
                for (auto depend : dag.rdep[job]) {
                    countOfDeps[depend]--;
                    if (countOfDeps[depend] == 0) {
                        actuallyReadyToBeExecuted.insert(depend);
                        executionQueue.erase(depend);
                    }
                }
                for (auto mtx : dag.mutOwn[job]) {
                    mutexState[mtx] = false;
                }
            }

        }
    }
}



}
