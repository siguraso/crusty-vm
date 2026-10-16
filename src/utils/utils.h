#ifndef UTILS_H
#define UTILS_H

#include "../class_file_parser/class_file_parser.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct slot_t slot_t;

typedef struct slot_t {
    uint32_t value;
    slot_t* next;
} slot_t;

typedef struct {
    slot_t* top;
} cvm_stack_t;

uint32_t read_u4(const uint8_t* buffer, size_t* offset);
uint16_t read_u2(const uint8_t* buffer, size_t* offset);
uint8_t read_u1(const uint8_t* buffer, size_t* offset);

slot_t* insert_slot(cvm_stack_t* stack, uint32_t value);
slot_t* pop_slot(cvm_stack_t* stack);

#endif
