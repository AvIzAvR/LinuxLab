#include "sortlib.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Invalid num of arguments. 2 arguments expecting\n");
        exit(-1);
    }

    FILE* binaryFile = fopen(argv[1], "rb");
    if (binaryFile == NULL) {
        printf("File openning error: %s", argv[2]);
        exit(-2);
    }

    header* data = (header*)malloc(sizeof(header));

    if (!fread(&data->numOfRecords, sizeof(uint64_t), 1, binaryFile)) {
        printf("Num of records reading error\n");
        fclose(binaryFile);
        exit(-3);
    }

    data->recordsArray = (record*)malloc(data->numOfRecords * sizeof(record));

    if (!fread(data->recordsArray, sizeof(record), data->numOfRecords, binaryFile)) {
        printf("Array of records reading error\n");
        fclose(binaryFile);
        exit(-3);
    }

    fclose(binaryFile);

    FILE* textFile = fopen(argv[2], "w");
    if (textFile == NULL) {
        printf("Error opening text file: %s\n", argv[2]);
        free(data->recordsArray);
        free(data);
        exit(-2);
    }

    for (int i = 0; i < data->numOfRecords; i++) {
        fprintf(textFile, "%lu   ", data->recordsArray[i].recno);
        fprintf(textFile, "%lf\n", data->recordsArray[i].timeMark);
    }
    
    fclose(textFile);

    free(data->recordsArray);
    free(data);

    return 0;
}