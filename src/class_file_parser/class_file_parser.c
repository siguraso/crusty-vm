#include "class_file_parser.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

int read_constant_pool_entries(constant_pool_entry_t* constant_pool_entries,
                               uint16_t count, size_t* offset,
                               const uint8_t* buffer) {
    for (uint16_t index = 1; index < count; index++) {
        uint8_t tag = read_u1(buffer, offset);
        constant_pool_entries[index].tag = tag;

        switch (tag) {
        case 7: // CONSTANT_Class
            constant_pool_entries[index].tag = tag;
            constant_pool_entries[index].class_info.name_index =
                read_u2(buffer, offset);
            break;

        case 9:  // CONSTANT_Fieldref
        case 10: // CONSTANT_Methodref
        case 11: // CONSTANT_InterfaceMethodref
            constant_pool_entries[index].tag = tag;
            constant_pool_entries[index].reference_info.class_index =
                read_u2(buffer, offset);
            constant_pool_entries[index].reference_info.name_and_type_index =
                read_u2(buffer, offset);
            break;

        case 8: // CONSTANT_String
            constant_pool_entries[index].tag = tag;
            constant_pool_entries[index].string_info.string_index =
                read_u2(buffer, offset);
            break;

        case 3: // CONSTANT_Integer
        case 4: // CONSTANT_Float
            constant_pool_entries[index].tag = tag;
            constant_pool_entries[index].integer_info.bytes =
                read_u4(buffer, offset);
            break;

        case 12: // CONSTANT_NameAndType
            constant_pool_entries[index].tag = tag;
            constant_pool_entries[index].name_type.name_index =
                read_u2(buffer, offset);
            constant_pool_entries[index].name_type.descriptor_index =
                read_u2(buffer, offset);
            break;

        case 1: { // CONSTANT_Utf8
            uint16_t length = read_u2(buffer, offset);

            // Requires a matching field in your union.
            constant_pool_entries[index].utf8_info.length = length;
            constant_pool_entries[index].utf8_info.bytes = malloc(length + 1);

            if (!constant_pool_entries[index].utf8_info.bytes) {
                printf("Memory allocation failed for Utf8 bytes.\n");
                return 0;
            }

            for (uint16_t byte = 0; byte < length; byte++) {
                constant_pool_entries[index].utf8_info.bytes[byte] =
                    read_u1(buffer, offset);
            }

            constant_pool_entries[index].utf8_info.bytes[length] = '\0';
            break;
        }

        default:
            printf("Unsupported constant-pool tag: %u\n", tag);
            return 0;
        }
    }

    return 1;
}

uint16_t* read_interfaces(const uint8_t* buffer, size_t* offset,
                          uint16_t interface_count) {
    uint16_t* interfaces = malloc(interface_count * sizeof(uint16_t));

    if (!interfaces) {
        printf("Memory allocation failed for interfaces.\n");
        return NULL;
    }

    for (int i = 0; i < interface_count; i++) {
        interfaces[i] = read_u2(buffer, offset);
    }

    return interfaces;
}

field_info_t* read_field_info(const uint8_t* buffer, size_t* offset,
                              uint16_t field_count) {
    field_info_t* fields = malloc(sizeof(field_info_t) * field_count);

    if (!fields) {
        printf("Memory allocation failed for fields.\n");
        return NULL;
    }

    for (uint16_t index = 0; index < field_count; index++) {
        fields[index].access_flags = read_u2(buffer, offset);
        fields[index].name_index = read_u2(buffer, offset);
        fields[index].descriptor_index = read_u2(buffer, offset);
        fields[index].attributes_count = read_u2(buffer, offset);

        if (fields[index].attributes_count > 0) {
            fields[index].attributes = malloc(sizeof(attribute_info_t) *
                                              (fields[index].attributes_count));

            if (!fields[index].attributes) {
                printf("Memory allocation failed for field attributes.\n");
                // Free previously allocated fields and their attributes
                for (uint16_t j = 0; j < index; j++) {
                    free(fields[j].attributes);
                }
                free(fields);
                return NULL;
            }

            // haha jindex
            for (uint16_t jindex = 0; jindex < fields[index].attributes_count;
                 jindex++) {
                attribute_info_t* current = &fields[index].attributes[jindex];

                current->name_index = read_u2(buffer, offset);
                current->length = read_u4(buffer, offset);

                if (current->length > 0) {

                    current->info = malloc(sizeof(uint8_t) * current->length);

                    if (!current->info) {
                        printf(
                            "Memory allocation failed for attribute info.\n");
                        // Free previously allocated attributes
                        for (uint16_t k = 0; k < jindex; k++) {
                            free(fields[index].attributes[k].info);
                        }
                        free(fields[index].attributes);
                        free(fields);
                        return NULL;
                    }

                    // haha kindex
                    for (uint32_t kindex = 0; kindex < current->length;
                         kindex++) {
                        current->info[kindex] = read_u1(buffer, offset);
                    }
                } else {
                    current->info = NULL;
                }
            }
        } else {
            fields[index].attributes = NULL;
        }
    }

    return fields;
}

method_info_t* read_method_info(const uint8_t* buffer, size_t* offset,
                                uint16_t method_count) {

    method_info_t* method_info = malloc(sizeof(method_info_t) * method_count);

    if (!method_info) {
        printf("Memory allocation failed for method_info.\n");
        return NULL;
    }

    for (size_t index = 0; index < method_count; index++) {
        method_info[index].access_flags = read_u2(buffer, offset);
        method_info[index].name_index = read_u2(buffer, offset);
        method_info[index].descriptor_index = read_u2(buffer, offset);

        uint16_t attributes_count = read_u2(buffer, offset);
        method_info[index].attributes_count = attributes_count;

        attribute_info_t* attributes =
            malloc(sizeof(attribute_info_t) * attributes_count);

        if (!attributes) {
            printf("Memory allocation failed for method attributes.\n");
            // Free previously allocated method_info and their attributes
            for (size_t j = 0; j < index; j++) {
                for (size_t k = 0; k < method_info[j].attributes_count; k++) {
                    free(method_info[j].attributes[k].info);
                }
                free(method_info[j].attributes);
            }
            free(method_info);
            return NULL;
        }

        for (size_t jindex = 0; jindex < attributes_count; jindex++) {
            attributes[jindex].name_index = read_u2(buffer, offset);

            uint32_t info_length = read_u4(buffer, offset);
            attributes[jindex].length = info_length;

            uint8_t* info = malloc(sizeof(uint8_t) * info_length);

            if (!info) {
                printf("Memory allocation failed for attribute info.\n");
                // Free previously allocated attributes
                for (size_t k = 0; k < jindex; k++) {
                    free(attributes[k].info);
                }
                free(attributes);
                // Free previously allocated method_info and their attributes
                for (size_t j = 0; j < index; j++) {
                    for (size_t k = 0; k < method_info[j].attributes_count;
                         k++) {
                        free(method_info[j].attributes[k].info);
                    }
                    free(method_info[j].attributes);
                }
                free(method_info);
                return NULL;
            }

            for (size_t kindex = 0; kindex < info_length; kindex++) {
                info[kindex] = read_u1(buffer, offset);
            }

            attributes[jindex].info = info;
        }

        method_info[index].attributes = attributes;
    }

    return method_info;
}

attribute_info_t* read_attribute_info(const uint8_t* buffer, size_t* offset,
                                      uint16_t attribute_count) {
    attribute_info_t* attributes =
        malloc(sizeof(attribute_info_t) * attribute_count);

    if (!attributes) {
        printf("Memory allocation failed for attributes.\n");
        return NULL;
    }

    for (uint16_t index = 0; index < attribute_count; index++) {
        attributes[index].name_index = read_u2(buffer, offset);
        attributes[index].length = read_u4(buffer, offset);

        if (attributes[index].length > 0) {
            attributes[index].info =
                malloc(sizeof(uint8_t) * attributes[index].length);

            if (!attributes[index].info) {
                printf("Memory allocation failed for attribute info.\n");
                // Free previously allocated attributes
                for (uint16_t j = 0; j < index; j++) {
                    free(attributes[j].info);
                }
                free(attributes);
                return NULL;
            }

            for (uint32_t jindex = 0; jindex < attributes[index].length;
                 jindex++) {
                attributes[index].info[jindex] = read_u1(buffer, offset);
            }
        } else {
            attributes[index].info = NULL;
        }
    }

    return attributes;
}

class_data_t* read_class_file(const char* file_path) {
    FILE* file_stream = fopen(file_path, "rb");

    if (!file_stream) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file_stream, 0, SEEK_END);
    long class_file_size = ftell(file_stream);
    fseek(file_stream, 0, SEEK_SET);

    unsigned char* buffer = malloc(class_file_size);

    if (!buffer) {
        printf("Memory allocation failed for buffer.\n");
        fclose(file_stream);
        return NULL;
    }

    fread(buffer, 1, class_file_size, file_stream);
    fclose(file_stream);

    // find the .class header piss
    size_t offset = 0;

    uint32_t magic = read_u4(buffer, &offset);

    if (!(buffer[0] == 0xCA && buffer[1] == 0xFE && buffer[2] == 0xBA &&
          buffer[3] == 0xBE)) {
        printf("not a valid class file :(\n");
        return NULL;
    }

    uint16_t minor_version = read_u2(buffer, &offset);
    uint16_t major_version = read_u2(buffer, &offset);
    uint16_t constant_pool_count = read_u2(buffer, &offset);

    constant_pool_entry_t* constant_pool_entries =
        malloc(sizeof(constant_pool_entry_t) * constant_pool_count);

    if (!read_constant_pool_entries(constant_pool_entries, constant_pool_count,
                                    &offset, buffer)) {
        free(constant_pool_entries);
        free(buffer);
        return NULL;
    }

    uint16_t access_flags = read_u2(buffer, &offset);
    uint16_t this_class = read_u2(buffer, &offset);
    uint16_t super_class = read_u2(buffer, &offset);
    uint16_t interface_count = read_u2(buffer, &offset);

    uint16_t* interfaces = read_interfaces(buffer, &offset, interface_count);

    uint16_t fields_count = read_u2(buffer, &offset);
    field_info_t* field_info = read_field_info(buffer, &offset, fields_count);

    if (!field_info) {
        for (uint16_t index = 0; index < fields_count; index++) {
            for (uint16_t jindex = 0;
                 jindex < field_info[index].attributes_count; jindex++) {
                free(field_info[index].attributes[jindex].info);
            }
            free(field_info[index].attributes);
        }
        free(field_info);

        return NULL;
    }

    uint16_t method_count = read_u2(buffer, &offset);
    method_info_t* method_info =
        read_method_info(buffer, &offset, method_count);

    if (!method_info) {
        for (uint16_t index = 0; index < method_count; index++) {
            for (uint16_t jindex = 0;
                 jindex < method_info[index].attributes_count; jindex++) {
                free(method_info[index].attributes[jindex].info);
            }
            free(method_info[index].attributes);
        }

        free(method_info);

        return NULL;
    }

    uint16_t attributes_count = read_u2(buffer, &offset);
    attribute_info_t* attributes =
        read_attribute_info(buffer, &offset, attributes_count);

    if (!attributes) {
        for (uint16_t index = 0; index < attributes_count; index++) {
            free(attributes[index].info);
        }
        free(attributes);

        return NULL;
    }

    free(buffer);

    class_data_t* class_file = malloc(sizeof(class_data_t));

    if (!class_file) {
        printf("Memory allocation failed for class_file.\n");
        return NULL;
    }

    class_file->magic = magic;
    class_file->minor_version = minor_version;
    class_file->major_version = major_version;
    class_file->constant_pool_count = constant_pool_count;
    class_file->constant_pool = constant_pool_entries;
    class_file->access_flags = access_flags;
    class_file->this_class = this_class;
    class_file->super_class = super_class;
    class_file->interfaces_count = interface_count;
    class_file->interfaces = interfaces;
    class_file->fields_count = fields_count;
    class_file->fields = field_info;
    class_file->methods_count = method_count;
    class_file->methods = method_info;
    class_file->attributes_count = attributes_count;
    class_file->attributes = attributes;

    return class_file;
}

int free_class_data(class_data_t* class_file) {
    if (!class_file) {
        return 1;
    }

    for (uint16_t index = 1; index < class_file->constant_pool_count; index++) {
        constant_pool_entry_t* entry = &class_file->constant_pool[index];

        if (entry->tag == 1) { // Utf8
            free(entry->utf8_info.bytes);
        }
    }
    free(class_file->constant_pool);
    free(class_file->interfaces);

    // Free fields
    for (uint16_t index = 0; index < class_file->fields_count; index++) {
        field_info_t* field = &class_file->fields[index];

        for (uint16_t jindex = 0; jindex < field->attributes_count; jindex++) {
            attribute_info_t* attribute = &field->attributes[jindex];
            free(attribute->info);
        }
        free(field->attributes);
    }
    free(class_file->fields);

    for (uint16_t index = 0; index < class_file->methods_count; index++) {
        method_info_t* method = &class_file->methods[index];

        for (uint16_t jindex = 0; jindex < method->attributes_count; jindex++) {
            attribute_info_t* attribute = &method->attributes[jindex];
            free(attribute->info);
        }
        free(method->attributes);
    }
    free(class_file->methods);

    for (uint16_t index = 0; index < class_file->attributes_count; index++) {
        attribute_info_t* attribute = &class_file->attributes[index];
        free(attribute->info);
    }
    free(class_file->attributes);

    free(class_file);

    return 0;
}
