#include "merge_sort.h"

namespace lab2 {

using TSegment = std::pair<size_t, size_t>;

static void Merge(std::vector<int> &vec, TSegment lseg, TSegment rseg, 
                  std::vector<int> &extra, size_t extraArrayStart) {
    size_t l1 = lseg.first, r1 = lseg.second, l2 = rseg.first, r2 = rseg.second;
    size_t i = l1, j = l2;
    size_t extraPtr = extraArrayStart;
    while (i <= r1 || j <= r2) {
        if (i > r1) {
            extra[extraPtr] = vec[j];
            j++;
        } else if (j > r2) {
            extra[extraPtr] = vec[i];
            i++;
        } else {
            if (vec[i] < vec[j]) {
                extra[extraPtr] = vec[i];
                i++;
            } else {
                extra[extraPtr] = vec[j];
                j++;
            }
        }
        extraPtr++;
    }
    // std::copy %%%
    for (size_t i = extraArrayStart; i < extraPtr; i++) {
        vec[l1 + i - extraArrayStart] = extra[i];
    }
}

void MergeSort(std::vector<int> &vec) {
    size_t size = vec.size();
    std::vector<int> extra(size);
    for (size_t k = 1; k < size; k *= 2) {
        // Segments of the array to merge
        for (int i = 0; i + k < size; i += 2 * k) {
            size_t l1, r1, l2, r2;
            l1 = i;
            r1 = i + k - 1;
            l2 = i + k;
            if (i + 2 * k - 1 < size - 1) {
                r2 = i + 2 * k - 1;
            } else {
                r2 = size - 1;
            }
            Merge(vec, {l1, r1}, {l2, r2}, extra, 0);
        }
    }
}

// ==================//
// Parallel sort
// ==================//

struct TMergeRawInputData {
    std::vector<int> *vec, *extra;
    TSegment lseg, rseg;
    size_t extraArrayStart;

    TMergeRawInputData(std::vector<int> *vec1, std::vector<int> *extra1, 
                       TSegment lseg1, TSegment rseg1, size_t extraArrayStart1) :
        vec(vec1), extra(extra1), lseg(lseg1), rseg(rseg1), extraArrayStart(extraArrayStart1) { }
};

static void* MergeRaw(void *data) {
    TMergeRawInputData *input = static_cast<TMergeRawInputData*>(data);
    Merge(*input->vec, input->lseg, input->rseg, *input->extra, input->extraArrayStart);
    return nullptr;
}

void ParallelMergeSort(std::vector<int> &vec, size_t threadCount) {
    using TTask = tp::TThreadPool::TTask;

    size_t size = vec.size();
    std::vector<int> extra(size);
    tp::TThreadPool tpool(threadCount);
    for (size_t k = 1; k < size; k *= 2) {
        // %%% <%OPT>
        if (k <= 2) {
            for (int i = 0; i + k < size; i += 2 * k) {
                size_t l1, r1, l2, r2;
                l1 = i;
                r1 = i + k - 1;
                l2 = i + k;
                if (i + 2 * k - 1 < size - 1) {
                    r2 = i + 2 * k - 1;
                } else {
                    r2 = size - 1;
                }
                Merge(vec, {l1, r1}, {l2, r2}, extra, 0);
            }
            continue;
        }
        // %% <%/OPT>
        // Segments of the array to merge
        int ptr = 0;
        for (int i = 0; i + k < size; i += 2 * k) {
            size_t l1, r1, l2, r2;
            size_t extraStart = i;
            l1 = i;
            r1 = i + k - 1;
            l2 = i + k;
            if (i + 2 * k - 1 < size - 1) {
                r2 = i + 2 * k - 1;
            } else {
                r2 = size - 1;
            }
            tpool.PushTask(ptr % threadCount, TTask{&MergeRaw, std::make_shared<TMergeRawInputData>
                                                    (TMergeRawInputData(&vec, 
                                                                        &extra, 
                                                                        {l1, r1}, 
                                                                        {l2, r2},
                                                                        extraStart))});
            ptr++;
        }
        tpool.Execute();
    }
    tpool.Terminate();
}

}
