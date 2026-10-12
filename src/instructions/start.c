#include "start.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>

static char* lookup_utf8(class_data_t* class_data, uint16_t index) {
    if (class_data == NULL || class_data->constant_pool == NULL ||
        index == 0 || index >= class_data->constant_pool_count) {
        return NULL;
    }

    constant_pool_entry_t* entry = &class_data->constant_pool[index];

    if (entry->tag != 1 || entry->utf8_info.bytes == NULL) {
        return NULL;
    }

    return entry->utf8_info.bytes;
}

method_info_t* locate_main(class_data_t* class_data) {
    if (class_data == NULL || class_data->methods == NULL) {
        return NULL;
    }

    for (uint16_t i = 0; i < class_data->methods_count; i++) {
        method_info_t* current_method = &class_data->methods[i];
        char* method_name = lookup_utf8(class_data, current_method->name_index);
        char* method_descriptor =
            lookup_utf8(class_data, current_method->descriptor_index);

        if (method_name == NULL || strcmp(method_name, "main") != 0) {
            continue;
        }

        if (method_descriptor == NULL ||
            strcmp(method_descriptor, "([Ljava/lang/String;)V") == 0 ||
            strcmp(method_descriptor, "(Ljava/lang/String;)V") == 0 ||
            (current_method->access_flags & 0x0009u) == 0x0009u) { // 0x0001 public and 0x0008 is static

                return current_method;
        }
    }

    return NULL;
}

code_attribute_t* parse_code_attribute(uint8_t* info) {
    size_t offset = 0;
    
    uint16_t max_stack = read_u2(info, &offset);
    uint16_t max_locals = read_u2(info, &offset);

    uint32_t code_length = read_u4(info, &offset);
    uint8_t* code = malloc(sizeof(uint8_t) * code_length);
    
    for (uint32_t i = 0; i < code_length; i++) {
        code[i] = read_u1(info, &offset);
    }

    code_attribute_t* attr = malloc(sizeof(code_attribute_t));

    attr->max_stack = max_stack;
    attr->max_locals = max_locals;
    attr->code_length = code_length;
    attr->code = code;

    return attr;
}

int start(class_data_t* class_data, method_info_t* entry_point) {
    if (class_data == NULL || entry_point == NULL) {
        return 1;
    }


    attribute_info_t* attributes = entry_point->attributes;

    for (uint16_t i = 0; i < entry_point->attributes_count; i++) {
        attribute_info_t curr = attributes[i];
        char* attribute_name = lookup_utf8(class_data, curr.name_index);

        if (attribute_name != NULL && strcmp(attribute_name, "Code") == 0) {
            code_attribute_t* code_attr = parse_code_attribute(curr.info);
            

            for (uint32_t j = 0; j < code_attr->code_length; j++) {
                printf("%x ", code_attr->code[j]);
            }

            free(code_attr->code);
            free(code_attr);
        }
    }

    return 0;
}