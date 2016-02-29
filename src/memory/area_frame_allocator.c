#include "area_frame_allocator.h"
#include "../multiboot2-elf64/memory_map.h"
#include "frame.h"

void fa_choose_next_area(struct AreaFrameAllocator *self);

struct AreaFrameAllocator fa_create(uint64_t kernel_start, uint64_t kernel_end,
        uint64_t multiboot_start, uint64_t multiboot_end,
        struct MemoryAreaIter memory_areas)
{
    struct AreaFrameAllocator allocator = {
        frame_containing_address(0),
        0,
        memory_areas,
        frame_containing_address(kernel_start),
        frame_containing_address(kernel_end),
        frame_containing_address(multiboot_start),
        frame_containing_address(multiboot_end)
    };

    fa_choose_next_area(&allocator);
    return allocator;
}

int fa_allocate_frame(struct AreaFrameAllocator *self, struct Frame *frame)
{
    struct MemoryArea *area = self->current_area;
    if (area != 0) {
        uint32_t next_free = { self->next_free_frame.number };

        // last frame of current area
        uint32_t current_area_last_frame =
            frame_containing_address( area->base_addr + area->length - 1).number;

        if (next_free > current_area_last_frame) {
            // all frames of current area are used, switch to next area
            fa_choose_next_area(self);
        }
        else if (next_free >= self->kernel_start.number &&
            next_free <= self->kernel_end.number) {
            // frame is used by the kernel
            struct Frame f = { self->kernel_end.number + 1 };
            self->next_free_frame = f;
        }
        else if (next_free >= self->multiboot_start.number &&
            next_free <= self->multiboot_end.number) {
            // frame is used by multiboot information structure
            struct Frame f = { self->multiboot_end.number + 1 };
            self->next_free_frame = f;
        }
        else {
            // frame is unused, increment `next_free_frame` and return it
            self->next_free_frame.number += 1;
            frame->number = next_free;
            return 1;
        }
        // frame was not valid, recursive call with updated `next_free_frame`
        return fa_allocate_frame(self, frame);
    }
    else {
        // return nothing
        return 0;
    }
}

void fa_deallocate_frame(struct AreaFrameAllocator *self, struct Frame *frame)
{

}

void fa_choose_next_area(struct AreaFrameAllocator *self)
{
    // clone iterator
    struct MemoryAreaIter iter = {
        self->areas.current_area,
        self->areas.last_area,
        self->areas.entry_size
    };

    // get area with lowes address that holds `addr` >= next_free_frame
    self->current_area = 0;
    uint64_t min_area = -1; // invalid
    int valid = iter.current_area->typ == 1;
    do {
        if (valid) {
            // last byte of that area
            uint64_t address = iter.current_area->base_addr +
                iter.current_area->length - 1;
            // last frame in that area
            struct Frame frame_at_addr = frame_containing_address(address);

            // check if that last frame is actually free
            if ( frame_at_addr.number >= self->next_free_frame.number) {
                // it's free, but is the memory area minimal?
                if (min_area == -1 || 
                        iter.current_area->base_addr < min_area) {
                    // found a better option
                    min_area = iter.current_area->base_addr;
                    self->current_area = iter.current_area;
                }
            }
        }
    } while ((valid = mb_get_next_memory_area(&iter)) != -1);

    if (self->current_area != 0) {
        struct Frame start_frame =
            frame_containing_address(self->current_area->base_addr);
        if (self->next_free_frame.number < start_frame.number) {
            // new area has lower base addr
            self->next_free_frame = start_frame;
        }
    }
}
