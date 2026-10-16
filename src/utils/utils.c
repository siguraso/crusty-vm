#include "utils.h"

// read the byte code datatypes
uint32_t read_u4(const uint8_t* buffer, size_t* offset) {
    uint32_t value = ((uint32_t)buffer[*offset] << 24) |
                     ((uint32_t)buffer[*offset + 1] << 16) |
                     ((uint32_t)buffer[*offset + 2] << 8) | buffer[*offset + 3];

    *offset += 4;
    return value;
}

uint16_t read_u2(const uint8_t* buffer, size_t* offset) {
    uint16_t value = ((uint16_t)buffer[*offset] << 8) | buffer[*offset + 1];

    *offset += 2;
    return value;
}

uint8_t read_u1(const uint8_t* buffer, size_t* offset) {
    return buffer[(*offset)++];
}

slot_t* insert_slot(cvm_stack_t* stack, uint32_t value) {
    slot_t* new_slot = (slot_t*)malloc(sizeof(slot_t));

    if (!new_slot) {
        return NULL;
    }

    new_slot->value = value;
    new_slot->next = stack->top;
    stack->top = new_slot;
    return new_slot;
}

slot_t* pop_slot(cvm_stack_t* stack) {
    if (!stack->top) {
        return NULL;
    }

    slot_t* popped_slot = stack->top;
    stack->top = stack->top->next;
    return popped_slot;
}
