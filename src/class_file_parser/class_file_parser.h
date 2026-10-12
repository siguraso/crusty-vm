#pragma once

#ifndef CLASS_FILE_PARSER_H
#define CLASS_FILE_PARSER_H

#include <stdint.h>

// structs to define methods and their names

typedef struct {
    uint8_t tag;

    union {
        struct {
            uint16_t name_index;
        } class_info;

        struct {
            uint16_t string_index;
        } string_info;

        struct {
            uint16_t name_index;
            uint16_t descriptor_index;
        } name_type;

        struct {
            uint32_t bytes;
        } integer_info;

        struct {
            uint16_t class_index;
            uint16_t name_and_type_index;
        } reference_info;

        struct {
            uint16_t length;
            char* bytes;
        } utf8_info;
    };
} constant_pool_entry_t;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;

    uint16_t attributes_count;
    attribute_info_t* attributes;
} method_info_t;

typedef struct {
    uint16_t name_index;
    uint32_t length;
    uint8_t* info;
} attribute_info_t;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info_t* attributes;

} field_info_t;

// main data struct
typedef struct {
    uint32_t magic; // 0xCAFEBABE
    uint16_t minor_version;
    uint16_t major_version;

    uint16_t constant_pool_count;
    constant_pool_entry_t* constant_pool;

    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    uint16_t interfaces_count;
    uint16_t* interfaces;

    uint16_t fields_count;
    field_info_t* fields;

    uint16_t methods_count;
    method_info_t* methods;

    uint16_t attributes_count;
    attribute_info_t* attributes;
} class_data_t;

class_data_t* read_class_file(const char* file_path);
int free_class_data(class_data_t* class_file);

#endif // CLASS_FILE_PARSER_H
