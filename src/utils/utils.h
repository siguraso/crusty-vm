

#ifndef UTILS_H
#define UTILS_H

#include "../class_file_parser/class_file_parser.h"
#include <stdint.h>
#include <stdlib.h>

uint32_t read_u4(const uint8_t* buffer, size_t* offset);
uint16_t read_u2(const uint8_t* buffer, size_t* offset);
uint8_t read_u1(const uint8_t* buffer, size_t* offset);

#endif
