#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "entry.h"
#include "table.h"
#include "../area_frame_allocator.h"
#include "../frame.h"

struct Page {
    uint32_t number;
};

PhysicalAdress pg_translate(VirtualAdress virtual_address);

struct Page pg_containing_address(VirtualAdress address);

uint64_t pg_start_address(struct Page *self);

/* 1 if succeeded, 0 if failed */
int pg_translate_page(struct Frame *frame, struct Page page);

uint32_t pg_p4_index(struct Page *page);
uint32_t pg_p3_index(struct Page *page);
uint32_t pg_p2_index(struct Page *page);
uint32_t pg_p1_index(struct Page *page);

void pg_map(struct Page *page, PageEntryFlags flags,
        struct AreaFrameAllocator *allocator);

void pg_mag_page_to_frame(struct Page *page, struct Frame *frame,
        PageEntryFlags flags, struct AreaFrameAllocator *allocator);

void pg_identity_map(struct Frame *frame, PageEntryFlags flags,
        struct AreaFrameAllocator *allocator);

void pg_unmap(struct Page *page, struct AreaFrameAllocator *allocator);

struct Table *pg_next_table_create(struct Table *self, tableindex index,
        struct AreaFrameAllocator *allocator);

#endif
