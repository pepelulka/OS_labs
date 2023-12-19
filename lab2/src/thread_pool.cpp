#ifdef _WIN32

#include "thread_pool.h"

namespace tp {

TThreadPool::TThreadPool(WORD threadNum) {
    isTerminated = false;
    this->counter.ResetAndSetTarget(threadNum);
    this->threadNum = threadNum;
    // Dangerous!! May be reallocation inside vector!
    this->threads.resize(threadNum);
    for (WORD i = 0; i < threadNum; i++) {
        this->threads[i].owner = this;
        this->threads[i].Start();
    }
    this->counter.WaitTarget();
    this->counter.ResetAndSetTarget(threadNum);
}

TThreadPool::~TThreadPool() { }

VOID TThreadPool::PushTask(WORD thread, TTask task) {
    assert(thread < threadNum);
    threads[thread].queue.push(task);
}

VOID TThreadPool::Execute() {
    for (auto &th : threads) {
        EnterCriticalSection(&th.csQueue);
        WakeConditionVariable(&th.cvQueue);
        LeaveCriticalSection(&th.csQueue);
    }
    this->counter.WaitTarget();
    this->counter.ResetAndSetTarget(threadNum);
}

VOID TThreadPool::Terminate() {
    isTerminated = true;
    Execute();
    for (auto &th : threads) {
        WaitForSingleObject(th.thread, INFINITE);
        CloseHandle(th.thread);
    }
}

unsigned WINAPI TThreadPool::TThread::ThreadRoutine(PVOID data) {
    TThread *thread = (TThread*)data;
    while (true) {
        EnterCriticalSection(&thread->csQueue);
        while (!thread->queue.empty()) {
            TTask task = thread->queue.front();
            thread->queue.pop();
            task.func(task.data.get());
        }
        if (thread->owner->isTerminated) {
            LeaveCriticalSection(&thread->csQueue);
            thread->owner->counter.Inc();
            return 0;
        }
        thread->owner->counter.Inc();
        SleepConditionVariableCS(&thread->cvQueue, &thread->csQueue, INFINITE);
        LeaveCriticalSection(&thread->csQueue);
    }
    return 0;
}

VOID TThreadPool::TThread::Start() {
    thread = (HANDLE)_beginthreadex(0, 0, &ThreadRoutine, this, 0, 0);
}

}

#endif
