#include "class_file_parser/class_file_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "instructions/start.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Please provide a path to a valid *.class file:"
               "\n ./crusty-vm <.class filepath>");

        return 1;
    }

    class_data_t* class_file = NULL;

    if (argv[1]) {
        class_file = read_class_file(argv[1]);

        if (!class_file) {
            return 2;
        }
    } else {
        printf("Problem reading arguments.");

        return 3;
    }

    start(class_file, locate_main(class_file));

    free_class_data(class_file);

    return 0;
}
