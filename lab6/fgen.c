#include "sortlib.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Invalid num of arguments. 2 arguments expecting\n");
        exit(-1);
    }

    header header;

    header.numOfRecords = atoi(argv[2]);
    if (header.numOfRecords % 256 != 0) {
        printf("Invalid number of records. Must be a multiple of 256\n");
        exit(-1);
    }

    srand(time(NULL));

    header.recordsArray = (record*)malloc(header.numOfRecords * sizeof(record));
    for (int i = 0; i < header.numOfRecords; i++) {
        header.recordsArray[i].recno = i + 1;
        header.recordsArray[i].timeMark = 15020 + rand() % (60420 - 15020 + 1);
        header.recordsArray[i].timeMark += 0.5 * ((rand() % 24) * 60 * 60 + (rand() % 60) * 60 + rand() % 60) / (12 * 60 * 60);
    }

    FILE* file = fopen(argv[2], "wb");
    if (file == NULL) {
        printf("File creating error: %s", argv[2]);
    } else {
        fwrite(&header.numOfRecords, sizeof(uint64_t), 1, file);
        fwrite(header.recordsArray, sizeof(record), header.numOfRecords, file);
    }

    fclose(file);
    free(header.recordsArray);
    return 0;
}