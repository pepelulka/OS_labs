#pragma once

#ifdef _WIN32

//========================//
// Windows                //
//========================//

#include <iostream>
#include <queue>
#include <functional>
#include <memory>
#include <cassert>
#include "windows.h"
#include "process.h"

namespace tp {

class TThreadPool final {
public:
    using TaskFunction = std::function<void(void*)>;

    struct TTask {
        TaskFunction func;
        void* data;
    };

    TThreadPool() = delete;
    TThreadPool(WORD threadNum);
    ~TThreadPool();

    void PushTask(WORD thread, TTask task);
    void Execute();
    void Terminate();
private:

    struct TThread {
        HANDLE thread;
        int a;
        TThreadPool *owner;
        int b;
        // Queue
        CRITICAL_SECTION csQueue;
        CONDITION_VARIABLE cvQueue;

        std::queue<TTask> queue;
        int c = 13;

        static unsigned WINAPI ThreadRoutine(PVOID data);
        void Start();

        TThread() {
            InitializeCriticalSection(&csQueue);
            InitializeConditionVariable(&cvQueue);
        }

        ~TThread() {
            DeleteCriticalSection(&csQueue);
        }
    }; 

    struct TCompletenessCounter {
        DWORD count, target;
        CRITICAL_SECTION cs;
        CONDITION_VARIABLE cv;

        TCompletenessCounter() { 
            InitializeCriticalSection(&cs);
            InitializeConditionVariable(&cv);
            count = 0;
        }

        ~TCompletenessCounter() {
            DeleteCriticalSection(&cs);
        }

        VOID WaitTarget() {
            EnterCriticalSection(&cs);
            if (count != target) {
                SleepConditionVariableCS(&cv, &cs, INFINITE);
            }

            LeaveCriticalSection(&cs);
        }

        VOID ResetAndSetTarget(DWORD target) {
            EnterCriticalSection(&cs);
            count = 0;
            this->target = target;
            LeaveCriticalSection(&cs);
        }

        VOID Inc() {
            EnterCriticalSection(&cs);
            count++;
            if (count == target) {
                WakeAllConditionVariable(&cv);
            }
            LeaveCriticalSection(&cs);
        }
    };

    WORD threadNum;
    BOOL isTerminated;
    std::vector<TThread> threads;
    TCompletenessCounter counter;
};

}

#endif
