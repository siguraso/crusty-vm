#include "start.h"
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
                printf("found main :O\n");
                return current_method;
        }
    }

    return NULL;
}