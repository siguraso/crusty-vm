#include "class_file.h"
#include <stdlib.h>
#include <stdio.h>

class_file_t* create_class_file(const char* file_path) {
    FILE* file_stream = fopen(file_path, "rb");

    if (!file_stream) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file_stream, 0, SEEK_END);
    long class_file_size = ftell(file_stream);
    fseek(file_stream, 0, SEEK_SET);

    unsigned char* buffer = malloc(class_file_size);

    fread(buffer, 1, class_file_size, file_stream);

    printf("buffer[0]: %x\n", buffer[0]);

    if (buffer[0] == 0xCA && buffer[1] == 0xFE && buffer[2] == 0xBA && buffer[3] == 0xBE) {
        printf("found cafebabe :3\n");
    } else {
        printf("not a valid class file :(\n");
    }

    class_file_t* class_file = malloc(sizeof(class_file_t)); 

    // fill the struct with the data from the buffer

    free(buffer);
    fclose(file_stream);

    return class_file;
}