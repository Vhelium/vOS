#ifndef AREA_FRAME_ALLOCATOR_H
#define AREA_FRAME_ALLOCATOR_H

#include "frame.h"
#include "../multiboot2-elf64/memory_map.h"

struct AreaFrameAllocator {
    struct Frame next_free_frame;
    struct MemoryArea *current_area;
    struct MemoryAreaIter areas;
    struct Frame kernel_start;
    struct Frame kernel_end;
    struct Frame multiboot_start;
    struct Frame multiboot_end;
};

/* returns new frame allocator */
struct AreaFrameAllocator fa_create(uint64_t kernel_start, uint64_t kernel_end,
        uint64_t multiboot_start, uint64_t multiboot_end,
        struct MemoryAreaIter memory_areas);

/* returns 1 if `frame`.number is set to next free frame
 * returns 0 if no free frame is available */
int fa_allocate_frame(struct AreaFrameAllocator *self, struct Frame *frame);

void fa_deallocate_frame(struct AreaFrameAllocator *self, struct Frame *frame);

#endif
