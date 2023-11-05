#ifdef _WIN32

#include "thread_pool.h"

namespace tp {

TThreadPool::TThreadPool(WORD threadNum) {
    isTerminated = false;
    this->counter.ResetAndSetTarget(threadNum);
    this->threadNum = threadNum;
    this->threads.resize(threadNum);
    for (auto &th : this->threads) {
        th.owner = this;
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

void WINAPI TThreadPool::TThread::ThreadRoutine(PVOID data) {
    // std::cout << "start\n";
    TThread *thread = (TThread*)data;
    while (true) {
        EnterCriticalSection(&thread->csQueue);
        // std::cout << "loop--\n";
        while (!thread->queue.empty()) {
            TTask task = thread->queue.front();
            thread->queue.pop();
            task.func(task.data.get());
            // std::cout << "task\n";
        }
        if (thread->owner->isTerminated) {
            LeaveCriticalSection(&thread->csQueue);
            thread->owner->counter.Inc();
            return ;
        }
        thread->owner->counter.Inc();
        // std::cout << "I sleep\n";
        SleepConditionVariableCS(&thread->cvQueue, &thread->csQueue, INFINITE);
        // std::cout << "Wake up\n";
        LeaveCriticalSection(&thread->csQueue);
    }
}

}

#endif
