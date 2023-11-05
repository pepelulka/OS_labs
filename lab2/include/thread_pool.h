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
        std::shared_ptr<void> data;
    };

    TThreadPool() = delete;
    TThreadPool(WORD threadNum);
    ~TThreadPool();

    // PushTask called only after WaitThreadPool(), otherwise - UB
    void PushTask(WORD thread, TTask task);
    void Execute();
    void Terminate();
private:

    struct TThread {
        TThreadPool *owner;
        HANDLE thread;
        // Queue
        CRITICAL_SECTION csQueue;
        CONDITION_VARIABLE cvQueue;

        std::queue<TTask> queue;

        static void WINAPI ThreadRoutine(PVOID data);

        TThread() {
            InitializeCriticalSection(&csQueue);
            InitializeConditionVariable(&cvQueue);
            thread = (HANDLE)_beginthread(&ThreadRoutine, 0, this);
        }

        ~TThread() {
            DeleteCriticalSection(&csQueue);
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
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
