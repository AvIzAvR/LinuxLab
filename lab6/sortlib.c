#include "sortlib.h"

int memsize;
int numOfBlocks;
int numOfThreads;
pthread_mutex_t mutex;
pthread_barrier_t barrier;
record* currentMemsizeBlock;

int compare(const void *a, const void *b) {
    return (double)((record*) a)->timeMark - (double)((record *) b)->timeMark;
}

void* sort_data(void* threadArgs_) {
    targ* args = (targ*)threadArgs_;
    pthread_barrier_wait(&barrier);

    while(currentMemsizeBlock < args->bufferBegin + memsize) {
        pthread_mutex_lock(&mutex);
        if(currentMemsizeBlock < args->bufferBegin + memsize) {
            record *temp = currentMemsizeBlock;
            currentMemsizeBlock += args->blockSize;
            pthread_mutex_unlock(&mutex);
            
            qsort(temp, args->blockSize, sizeof(record), compare);
        } else {
            pthread_mutex_unlock(&mutex);
            pthread_barrier_wait(&barrier);
            break;
        }
    }

    merge(args);
}

void merge(void* threadArgs_) {
    targ* args = (targ*)threadArgs_;
    int mergeStep = 2;

    while (mergeStep <= numOfBlocks) {   
        pthread_barrier_wait(&barrier);
        currentMemsizeBlock = args->bufferBegin;

        while (currentMemsizeBlock < args->bufferBegin + memsize) {
            pthread_mutex_lock(&mutex);

            if (currentMemsizeBlock < args->bufferBegin + memsize) {
                record *temp = currentMemsizeBlock;
                currentMemsizeBlock += mergeStep * args->blockSize;
                pthread_mutex_unlock(&mutex);

                int sizeOfOneBlockToMerge = (mergeStep / 2) * args->blockSize;
                merge_blocks(temp, sizeOfOneBlockToMerge);
            } else {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
        mergeStep *= 2;
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
}

void merge_blocks(record* firstBlockBegin, int sizeOfOneBlockToMerge) {
    record *left = (record *)malloc(sizeOfOneBlockToMerge * sizeof(record));
    record *right = (record *)malloc(sizeOfOneBlockToMerge * sizeof(record));
    memcpy(left, firstBlockBegin, sizeOfOneBlockToMerge * sizeof(record));
    memcpy(right, firstBlockBegin + sizeOfOneBlockToMerge, sizeOfOneBlockToMerge * sizeof(record));
    int leftIdx = 0;
    int rightIdx = 0;
    int idx = 0;

    while (leftIdx < sizeOfOneBlockToMerge && rightIdx < sizeOfOneBlockToMerge) {
        if (left[leftIdx].timeMark > right[rightIdx].timeMark) {
            firstBlockBegin[idx].timeMark = right[rightIdx].timeMark;
            firstBlockBegin[idx].recno = right[rightIdx].recno;
            rightIdx++;
            idx++;
        } else {
            firstBlockBegin[idx].timeMark = left[leftIdx].timeMark;
            firstBlockBegin[idx].recno = left[leftIdx].recno;
            leftIdx++;
            idx++;
        }
    }

    while (leftIdx < sizeOfOneBlockToMerge) {
        firstBlockBegin[idx].timeMark = left[leftIdx].timeMark;
        firstBlockBegin[idx].recno = left[leftIdx].recno;
        leftIdx++;
        idx++;
    }

    while (rightIdx < sizeOfOneBlockToMerge) {
        firstBlockBegin[idx].timeMark = right[rightIdx].timeMark;
        firstBlockBegin[idx].recno = right[rightIdx].recno;
        rightIdx++;
        idx++;
    }
    
    free(left);
    free(right);
}