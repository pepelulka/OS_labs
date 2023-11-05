#ifdef _WIN32

// %%%
#define LOG(X) std::cout << X << std::endl

//========================//
// Windows                //
//========================//

#include "task_queue.h"

namespace tq {

TTaskQueue::TTaskQueue() {
    InitializeCriticalSection(&this->queueCriticalSection);
    InitializeConditionVariable(&this->condVar);
}

TTaskQueue::~TTaskQueue () {
    DeleteCriticalSection(&this->queueCriticalSection);
}

VOID TTaskQueue::Terminate() {
    EnterCriticalSection(&this->queueCriticalSection);
    this->isTerminated = true;
    WakeAllConditionVariable(&this->condVar);
    LeaveCriticalSection(&this->queueCriticalSection);
}

VOID TTaskQueue::PushTask(TTask task) {
    EnterCriticalSection(&this->queueCriticalSection);
    this->queue.push(task);
    WakeConditionVariable(&this->condVar);
    LeaveCriticalSection(&this->queueCriticalSection);
}

bool TTaskQueue::ExecuteTask() {
    EnterCriticalSection(&this->queueCriticalSection);
    if (this->queue.empty() && !this->isTerminated) {
        SleepConditionVariableCS(&this->condVar, &this->queueCriticalSection, INFINITE);
    }
    if (this->queue.empty() && this->isTerminated) {
        LeaveCriticalSection(&this->queueCriticalSection);
        return false;
    }
    TTask task = this->queue.front();
    this->queue.pop();
    LeaveCriticalSection(&this->queueCriticalSection);
    task.func(task.data.get());
    return true;
}

static unsigned int WINAPI ThreadRoutine(void *data) {
    // LOG("start thread");
    TTaskQueue* queue = (TTaskQueue*)data;
    while (queue->ExecuteTask()) {
        // LOG("Task completed");
    }
    // LOG("End");
    return (DWORD)0;
}

VOID ExecuteThreadPool(TTaskQueue &queue, DWORD threadNum) {
    std::vector<HANDLE> threads(threadNum);
    for (DWORD i = 0; i < threadNum; i++) {
        threads[i] = (HANDLE)_beginthreadex(0, 0, &ThreadRoutine, (PVOID)&queue, 0, 0);
    }
    for (DWORD i = 0; i < threadNum; i++) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }
}

}

#else

#endif
