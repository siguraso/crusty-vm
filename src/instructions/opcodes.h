#ifndef OPCODES_H
#define OPCODES_H

#include "../class_file_parser/class_file_parser.h"
#include "../utils/utils.h"
#include "start.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
    GETSTATIC = 0xB2,
    LDC = 0x12,

} opcode_t;

void parse_opcodes(uint8_t opcode, class_data_t* class_data,
                   code_attribute_t* code_attr, size_t* curr_index,
                   cvm_stack_t* stack);

#endif
