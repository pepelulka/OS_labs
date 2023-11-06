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

void TThreadPool::PushTask(WORD thread, TTask task) {
    assert(thread < threadNum);
    threads[thread].queue.push(task);
}

void TThreadPool::Execute() {
    for (auto &th : threads) {
        EnterCriticalSection(&th.csQueue);
        WakeConditionVariable(&th.cvQueue);
        LeaveCriticalSection(&th.csQueue);
    }
    this->counter.WaitTarget();
    this->counter.ResetAndSetTarget(threadNum);
}

void TThreadPool::Terminate() {
    isTerminated = true;
    // std::cout << "Termination!\n";
    Execute();
    // std::cout << "Yeah bitch\n";
    for (auto &th : threads) {
        WaitForSingleObject(th.thread, INFINITE);
        CloseHandle(th.thread);
    }
}

unsigned WINAPI TThreadPool::TThread::ThreadRoutine(PVOID data) {
    // std::cout << "start\n";
    TThread *thread = (TThread*)data;
    while (true) {
        EnterCriticalSection(&thread->csQueue);
        // std::cout << "loop--\n";
        while (!thread->queue.empty()) {
            // std::cout << "Meow " << GetCurrentThreadId() << std::endl;
            TTask task = thread->queue.front();
            thread->queue.pop();
            task.func(task.data);
            // std::cout << "Meow end" << GetCurrentThreadId() << std::endl;
            // std::cout << "task\n";
        }
        if (thread->owner->isTerminated) {
            LeaveCriticalSection(&thread->csQueue);
            thread->owner->counter.Inc();
            return 0;
        }
        thread->owner->counter.Inc();
        // std::cout << "I sleep\n";
        SleepConditionVariableCS(&thread->cvQueue, &thread->csQueue, INFINITE);
        // std::cout << "Wake up\n";
        LeaveCriticalSection(&thread->csQueue);
    }
    return 0;
}

void TThreadPool::TThread::Start() {
    thread = (HANDLE)_beginthreadex(0, 0, &ThreadRoutine, this, 0, 0);
}

}

#endif
