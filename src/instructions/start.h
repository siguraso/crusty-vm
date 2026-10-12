#pragma once

#ifndef START_H
#define START_H

#include "../class_file_parser/class_file_parser.h"
#include <stdlib.h>

typedef struct {
    uint16_t max_stack;
    uint16_t max_locals;
    
    uint32_t code_length;
    uint8_t* code;

    // might implement l8r

    // uint16_t exceptions_legth;
    // exception_t* exceptions;

    // uint16_t attributes_count;
    // attribute_info_t* attributes;
} code_attribute_t;

method_info_t* locate_main(class_data_t* class_data);

// idk how to implement this quite yet
int start(class_data_t* class_data, method_info_t* entry_point);

#endif // START_H
