#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <stdint.h>

struct MemoryArea {
   uint64_t base_addr;
   uint64_t length;
   uint32_t typ;
   uint32_t reserved;
} __attribute__((packed));

struct MemoryMapTag {
    uint32_t typ;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct MemoryArea first_area;
} __attribute__((packed));

struct MemoryAreaIter {
    struct MemoryArea *current_area;
    struct MemoryArea *last_area;
    uint32_t entry_size;
};

/* returns Iterator for MemoryAreas */
struct MemoryAreaIter mb_get_memory_areas(struct MemoryMapTag *mmt);

/* go to next item in Iterator `iter` */
int mb_get_next_memory_area(struct MemoryAreaIter *iter);

#endif
