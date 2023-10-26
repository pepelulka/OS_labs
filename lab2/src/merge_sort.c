#include "merge_sort.h"

#include <pthread.h>
#include <unistd.h>

static void Merge(int *array, size_t size,
                  size_t l1, size_t r1, size_t l2, size_t r2, int *extra) {
    assert(l1 < size && l2 < size && r1 < size && r2 < size);
    assert(l2 == r1 + 1);
    size_t i = l1, j = l2;
    size_t extraPtr = 0;
    while (i <= r1 || j <= r2) {
        if (i > r1) {
            extra[extraPtr] = array[j];
            j++;
        } else if (j > r2) {
            extra[extraPtr] = array[i];
            i++;
        } else {
            if (array[i] < array[j]) {
                extra[extraPtr] = array[i];
                i++;
            } else {
                extra[extraPtr] = array[j];
                j++;
            }
        }
        extraPtr++;
    }
    // memcpy(array + l1, extra, extraPtr * sizeof(int));
    for (size_t i = 0; i < extraPtr; i++) {
        array[l1 + i] = extra[i];
    }
}

void MergeSort(int *array, size_t size, int *extra) {
    if (size == 0 || size == 1) {
        return ;
    }
    size_t mid = size / 2;
    MergeSort(array, mid, extra);
    MergeSort(array + mid, size - mid, extra);
    Merge(array, size, 0, mid - 1, mid, size - 1, extra);
}

// %%% STATIC %%% STATIC %%% STATIC %%% STATIC %%% STATIC %%%

typedef struct {
    int *array, *extra;
    size_t size, l1, r1, l2, r2;
} TMergeInputData;

// Task queue

typedef struct TQN {
    TMergeInputData data;
    struct TQN *next;
} TTaskQueueNode;

typedef struct {
    size_t size;
    TTaskQueueNode *first, *last;
    bool isTerminated;
    // Multi-thread stuff
    // Firstly you must lock mutexForModification and then mutexForWorkers!!! (In order to avoid dead locks)
    pthread_mutex_t mutexForWorkers; // When you want to use queue, just lock its mutex, but do not unlock if size == 0
    pthread_mutex_t mutexForZeroCount;
    pthread_cond_t condVarForWorkers;
    pthread_cond_t condVarForZeroCount;
    pthread_mutex_t mutexForModification;
} TTaskQueue;

static TTaskQueueNode *TQNCreate(TMergeInputData data) {
    TTaskQueueNode *result = malloc(sizeof(TTaskQueueNode));
    result->data = data;
    result->next = NULL;
    return result;
}

static void TQNDestroy(TTaskQueueNode *node) {
    if (node != NULL) {
        free(node);
    }
}

TTaskQueue* TaskQueueCreate() {
    TTaskQueue *queue = malloc(sizeof(TTaskQueue));
    queue->first = NULL;
    queue->size = 0;
    queue->isTerminated = false;
    pthread_cond_init(&queue->condVarForWorkers, NULL);
    pthread_cond_init(&queue->condVarForZeroCount, NULL);
    pthread_mutex_init(&queue->mutexForWorkers, NULL);
    pthread_mutex_init(&queue->mutexForZeroCount, NULL);
    pthread_mutex_init(&queue->mutexForModification, NULL);
    return queue;
}

void TaskQueueTerminate(TTaskQueue *queue) {
    pthread_mutex_lock(&queue->mutexForModification);
    queue->isTerminated = true;
    pthread_cond_broadcast(&queue->condVarForWorkers);
    pthread_mutex_unlock(&queue->mutexForModification);
}

void TaskQueueDestroy(TTaskQueue *queue) {
    TTaskQueueNode *node = queue->first;
    if (node != NULL) {
        while (node->next != NULL) {
            TTaskQueueNode *temp = node;
            TQNDestroy(temp);
            node = node->next;
        }
        TQNDestroy(node);
    }
    pthread_mutex_destroy(&queue->mutexForModification);
    pthread_mutex_destroy(&queue->mutexForWorkers);
    pthread_mutex_destroy(&queue->mutexForZeroCount);
    pthread_cond_destroy(&queue->condVarForWorkers);
    pthread_cond_destroy(&queue->condVarForZeroCount);
    free(queue);
}

void TaskQueuePush(TTaskQueue *queue, TMergeInputData data) {
    pthread_mutex_lock(&queue->mutexForModification);
    TTaskQueueNode *node = TQNCreate(data);
    if (queue->first == NULL) {
        queue->first = node;
        queue->last = node;
    } else if (queue->first == queue->last) {
        queue->last = node;
        queue->first->next = queue->last;
    } else {
        queue->last->next = node;
        queue->last = node;
    }
    queue->size++;
    pthread_cond_signal(&queue->condVarForWorkers);
    pthread_mutex_unlock(&queue->mutexForModification);
}

bool TaskQueuePop(TTaskQueue *queue, TMergeInputData *resultPtr) {
    pthread_mutex_lock(&queue->mutexForModification);
    while (queue->size == 0 && queue->isTerminated != true) {
        printf("{\n");
        pthread_cond_wait(&queue->condVarForWorkers, &queue->mutexForModification);
        printf("}\n");
    }
    if (queue->isTerminated) {
        pthread_mutex_unlock(&queue->mutexForModification);
        return false;
    }
    if (queue->size == 0) {
        assert(false && "Something went wrong!");
    }
    TMergeInputData result;
    if (queue->size == 1) {
        result = queue->first->data;
        TQNDestroy(queue->first);
        queue->first = NULL;
        queue->last = NULL;
    } else if (queue->size == 2) {
        result = queue->first->data;
        TQNDestroy(queue->first);
        queue->first = queue->last;
    } else {
        result = queue->first->data;
        TTaskQueueNode *temp = queue->first;
        queue->first = queue->first->next;
        TQNDestroy(temp);
    }
    queue->size--;
    if (queue->size == 0) {
        pthread_cond_signal(&queue->condVarForZeroCount);
    }
    pthread_mutex_unlock(&queue->mutexForModification);
    *resultPtr = result;
    return true;
}

void WaitUntilAllTaskDone(TTaskQueue *queue) {
    pthread_mutex_lock(&queue->mutexForModification);
    pthread_cond_wait(&queue->condVarForZeroCount, &queue->mutexForModification);
    pthread_mutex_unlock(&queue->mutexForModification);
}

// Task queue <<END>>

// Thread routine

void *WorkerRoutine(void *data) {
    TTaskQueue *queue = (TTaskQueue*)data;
    TMergeInputData task;
    while (TaskQueuePop(queue, &task)) {
        Merge(task.array, task.size, task.l1, task.r1,
              task.l2, task.r2, task.extra);
    }
    return NULL;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void ParallelMergeSort(int *array, size_t size, size_t threadCount) {
    int *extra = malloc(2 * sizeof(int) * size); // Two sizes of array
    // Threading
    TTaskQueue *queue = TaskQueueCreate();
    pthread_t *threads = malloc(sizeof(pthread_t) * threadCount);
    for (size_t i = 0; i < threadCount; i++) {
        pthread_create(&threads[i], NULL, WorkerRoutine, queue);
    }
    //
    TMergeInputData input;
    input.array = array;
    input.size = size;
    //
    for (size_t k = 1; k < size; k *= 2) {
        // Segments of the array to merge
        for (int i = 0; i + k < size; i += 2 * k) {
            input.l1 = i;
            input.r1 = i + k - 1;
            input.l2 = i + k;
            if (i + 2 * k - 1 < size - 1) {
                input.r2 = i + 2 * k - 1;
            } else {
                input.r2 = size - 1;
            }
            input.extra = extra + 2 * i;
            TaskQueuePush(queue, input);
        }
        WaitUntilAllTaskDone(queue);
    }
    TaskQueueTerminate(queue);
    for (size_t i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
    TaskQueueDestroy(queue);
    // Adding array to
}
