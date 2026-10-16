#include "opcodes.h"
#include "../utils/utils.h"
#include "start.h"
#include <stdio.h>

void execute_opcodes(uint8_t opcode, class_data_t* class_data,
                     code_attribute_t* code_attr, size_t* curr_index,
                     cvm_stack_t* stack) {

    switch (opcode) {
    case GETSTATIC: {
        (*curr_index)++;
        uint16_t index = read_u2(code_attr->code, curr_index);

        printf("GETSTATIC: index = %u\n", index & 0xFFFF);

        insert_slot(stack, index & 0xFFFF);

        break;
    }
    case LDC: {
        (*curr_index)++; // move past opcode byte
        // ignore for now
        read_u1(code_attr->code, curr_index);
        break;
    }
    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        (*curr_index)++; // move past unknown opcode byte
        break;
    }
}
