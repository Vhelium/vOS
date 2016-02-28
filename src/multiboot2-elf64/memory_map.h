#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <stdint.h>

struct MemoryArea {
   int64_t base_addr;
   int64_t length;
   int32_t typ;
   int32_t reserved;
} __attribute__((packed));

struct MemoryMapTag {
    int32_t typ;
    int32_t size;
    int32_t entry_size;
    int32_t entry_version;
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
