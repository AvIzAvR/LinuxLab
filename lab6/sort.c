#include "sortlib.h"


int main(int argc, char *argv[]) {

    if (argc != 5) {
        printf("Invalid num of arguments. 4 arguments expecting\n");
        exit(-1);
    }

    memsize = atoi(argv[1]);
    numOfBlocks = atoi(argv[2]);
    numOfThreads = atoi(argv[3]) - 1;
    
    if(numOfThreads < 8 || numOfThreads > 8000) {
        printf("Invalid threads count. Must be from 8 to 8000\n");
        exit(-1);
    }
    if(numOfBlocks % 2 || numOfBlocks < numOfThreads) {
        printf("Invalid blocks count. Must be a multiple of 2 and more than threads count\n");
        exit(-1);
    }
    
    const char *fileName = argv[4];
    int blockSize = memsize / numOfBlocks;

    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-2);
    }
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    int fileDescriptor = fileno(file);


    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, numOfThreads);

    void* bufferBegin = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);

    bufferBegin += sizeof(uint64_t);

    currentMemsizeBlock = (record*)bufferBegin;

    pthread_t childPthreadIds[numOfThreads - 1];

    for (int i = 1; i < numOfThreads; i++) {
        targ* args = (targ*)malloc(sizeof(targ));
        args->blockSize = blockSize;
        args->bufferBegin = (record*)bufferBegin;
        args->threadNum = i;
        pthread_create(&childPthreadIds[i - 1], NULL, sort_data, args);
    }

    targ* args = (targ*)malloc(sizeof(targ));
    args->blockSize = blockSize;
    args->bufferBegin = (record*)bufferBegin;
    args->threadNum = 0;

    sort_data(args);

    for(int i = 1; i < numOfThreads; i++) {
        pthread_join(childPthreadIds[i - 1], NULL);
    }

    munmap(bufferBegin - sizeof(uint64_t), fileSize);

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);
    
    fclose(file);

    return 0;
}