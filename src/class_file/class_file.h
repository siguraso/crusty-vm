#pragma once

#ifndef CLASS_FILE_H
#define CLASS_FILE_H

#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t minor_version;
    uint32_t major_version;
    uint32_t constant_pool_count;
} class_file_t;

class_file_t* create_class_file(const char* file_path);

#endif // CLASS_FILE_H
