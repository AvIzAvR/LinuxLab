#ifndef LIB_H
#define LIB_H

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double timeMark;
    uint64_t recno;
} record;

typedef struct {
    uint64_t numOfRecords;
    record* recordsArray;
} header;

typedef struct {
    record* bufferBegin;
    int blockSize;
    int threadNum;
} targ;   


extern int memsize;
extern int numOfBlocks;
extern int numOfThreads;

extern pthread_barrier_t barrier;
extern pthread_mutex_t mutex;
extern record* currentMemsizeBlock;

int compare(const void *a, const void *b);
void* sort_data(void* threadArgs_);
void merge(void* threadArgs_);
void merge_blocks(record* firstBlockBegin, int sizeOfOneBlockToMerge);

#endif