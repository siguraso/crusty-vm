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
