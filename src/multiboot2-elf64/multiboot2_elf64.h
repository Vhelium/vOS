#ifndef MULTIBOOT2_ELF64_H
#define MULTIBOOT2_ELF64_H

#include <stdint.h>

struct Tag {
    int32_t typ;
    int32_t size;
    // tag specific fields
};

struct BootInformation {
    int32_t total_size;
    int32_t _reserved;
    struct Tag first_tag;
};

struct BootInformation *mb_load(int32_t adress);

int mb_has_valid_end_tag(struct BootInformation *bi);

struct MemoryMapTag *mb_get_memory_map_tag(struct BootInformation *bi);

#endif
