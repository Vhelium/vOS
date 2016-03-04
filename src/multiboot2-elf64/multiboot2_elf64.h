#ifndef MULTIBOOT2_ELF64_H
#define MULTIBOOT2_ELF64_H

#include <stdint.h>

struct Tag {
    uint32_t typ;
    uint32_t size;
    // tag specific fields
};

struct BootInformation {
    uint32_t total_size;
    uint32_t _reserved;
    struct Tag first_tag;
};

struct BootInformation *mb_load(uint32_t adress);

int mb_has_valid_end_tag(struct BootInformation *bi);

struct MemoryMapTag *mb_get_memory_map_tag(struct BootInformation *bi);

struct ElfSectionsTag *mb_get_elf_sections_tag(struct BootInformation *bi);

#endif
