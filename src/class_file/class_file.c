#include "class_file.h"
#include <stdio.h>
#include <stdlib.h>

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

class_data_t* create_class_file(const char* file_path) {
    FILE* file_stream = fopen(file_path, "rb");

    if (!file_stream) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file_stream, 0, SEEK_END);
    long class_file_size = ftell(file_stream);
    fseek(file_stream, 0, SEEK_SET);

    unsigned char* buffer = malloc(class_file_size);

    fread(buffer, 1, class_file_size, file_stream);
    fclose(file_stream);

    // find the .class header piss
    size_t offset = 0;

    uint32_t magic = read_u4(buffer, &offset);

    if (buffer[0] == 0xCA && buffer[1] == 0xFE && buffer[2] == 0xBA &&
        buffer[3] == 0xBE) {
        printf("found cafebabe :3\n");
    } else {
        printf("not a valid class file :(\n");

        return NULL;
    }

    uint16_t minor_version = read_u2(buffer, &offset);
    uint16_t major_version = read_u2(buffer, &offset);
    uint16_t constant_pool_count = read_u2(buffer, &offset);

    // fins constant pool
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

    free(buffer);

    class_data_t* class_file = malloc(sizeof(class_data_t));

    class_file->magic = magic;
    class_file->minor_version = minor_version;
    class_file->major_version = major_version;
    class_file->constant_pool_count = constant_pool_count;
    class_file->constant_pool = constant_pool_entries;
    class_file->access_flags = access_flags;
    class_file->this_class = this_class;
    class_file->super_class = super_class;

    // debug print
    printf("Class file created successfully:\n");
    printf("Magic: 0x%X\n", class_file->magic);
    printf("Minor Version: %u\n", class_file->minor_version);
    printf("Major Version: %u\n", class_file->major_version);
    printf("Constant Pool Count: %u\n", class_file->constant_pool_count);

    for (uint16_t index = 1; index < class_file->constant_pool_count; index++) {
        constant_pool_entry_t* entry = &class_file->constant_pool[index];

        printf("  #%u tag=%u", index, entry->tag);

        switch (entry->tag) {
        case 1:
            printf(" Utf8 length=%u value=\"%s\"", entry->utf8_info.length,
                   entry->utf8_info.bytes);
            break;
        case 3:
        case 4:
            printf(" bytes=0x%08X", entry->integer_info.bytes);
            break;
        case 7:
            printf(" name_index=%u", entry->class_info.name_index);
            break;
        case 8:
            printf(" string_index=%u", entry->string_info.string_index);
            break;
        case 9:
        case 10:
        case 11:
            printf(" class_index=%u name_and_type_index=%u",
                   entry->reference_info.class_index,
                   entry->reference_info.name_and_type_index);
            break;
        case 12:
            printf(" name_index=%u descriptor_index=%u",
                   entry->name_type.name_index,
                   entry->name_type.descriptor_index);
            break;
        default:
            printf(" unsupported");
            break;
        }

        printf("\n");
    }

    return class_file;
}
