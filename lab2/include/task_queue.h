#pragma once

#ifdef _WIN32

//========================//
// Windows                //
//========================//

#include <iostream>
#include <queue>
#include <functional>
#include <memory>
#include "windows.h"
#include "process.h"

namespace tq {

class TTaskQueue final {
public:
    using TaskFunction = std::function<void*(void*)>;

    struct TTask {
        TaskFunction func;
        std::shared_ptr<void> data;
    };

    TTaskQueue ();
    ~TTaskQueue ();

    TTaskQueue (TTaskQueue &other) = delete;
    TTaskQueue operator= (TTaskQueue &other) = delete;

    VOID Terminate();
    VOID PushTask(TTask task);
    // Execute task on top of queue or wait if there are no tasks.
    // Returns false if queue is terminated. (True if task has been done succesfully).
    bool ExecuteTask();

private:
    CRITICAL_SECTION queueCriticalSection;
    CONDITION_VARIABLE condVar;

    std::queue<TTask> queue;
    bool isTerminated;
};

VOID ExecuteThreadPool(TTaskQueue &queue, DWORD threadNum);

}

#else

#endif

