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
    static int Exec(const std::string& path);
};

struct LogStack {
    // Stack of completed jobs
    std::vector<std::string> completed;
    size_t wasRead = 0;

    std::mutex mut;
    std::condition_variable cv;

    void Push(const std::string &str);

};

class TBasicExecutor {
private:
    const constexpr static size_t STANDART_MAX_PROC_COUNT = 4;

    size_t maxProcCount;
    LogStack * log;

    size_t procCount = 0;
    std::mutex mpcountMutex;
    std::condition_variable mpcountCondVar;

    void RawExecute(const std::string &name, const std::string &path, LogStack *log);

public:

    TBasicExecutor(LogStack *_log) : maxProcCount(STANDART_MAX_PROC_COUNT), log(_log) { }
    TBasicExecutor(size_t mpCount, LogStack *_log) : maxProcCount(mpCount), log(_log) { }

    void Execute(const std::string &path, const std::string &name);

};

class TDagJobExecutor {
private:

    const static size_t STANDART_MAX_PROC_COUNT = 4;
    size_t maxProcCount;

    size_t target, current;

    size_t procCount;
    std::set<std::string> actuallyReadyToBeExecuted;
    std::mutex mut; // procCount and sets
    std::condition_variable cv;

    LogStack log;
    TBasicExecutor ex;

public:

    TDagJobExecutor() : maxProcCount(STANDART_MAX_PROC_COUNT), ex(maxProcCount, &log) { }
    TDagJobExecutor(size_t mpCount) : maxProcCount(mpCount), ex(maxProcCount, &log) { }

    bool Execute(TJobDag &dag);

};

}
