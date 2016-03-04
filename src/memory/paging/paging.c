#include "paging.h"
#include "table.h"
#include "entry.h"
#include "../frame.h"
#include "../area_frame_allocator.h"
#include "../../vlib.h"
#include "../../vio.h"

PhysicalAdress pg_translate(VirtualAdress virtual_address)
{
    uint64_t offset = virtual_address % PAGE_SIZE;
    struct Frame f;
    if(pg_translate_page(&f, pg_containing_address(virtual_address)))
        return f.number * PAGE_SIZE + offset;
    else
        //TODO: handle it somehow^^
        return -1; // invalid address
}

struct Page pg_containing_address(VirtualAdress address)
{
    assert(address < 0x0000800000000000U || address >= 0xffff800000000000U);
    struct Page p = { address / PAGE_SIZE };
    return p;
}

uint64_t pg_start_address(struct Page *self)
{
    // TODO 32bit to 64bit
    return self->number * PAGE_SIZE;
}

int pg_translate_page(struct Frame *frame, struct Page page)
{
    struct Table *p3 = 0;
    struct Table *p2 = 0;
    struct Table *p1 = 0;

    p3 = pg_table_next_table(P4_TABLE, pg_p4_index(&page));
    if(!p3) {
        //printf("p3 null!\n");
        goto TABLE_ZERO;
    }

    p2 = pg_table_next_table(p3, pg_p3_index(&page));
    if(!p2) {
        //printf("p2 null!\n");
        goto TABLE_ZERO;
    }

    p1 = pg_table_next_table(p2, pg_p2_index(&page));
    if(!p1) {
        //printf("p1 null!\n");
        goto TABLE_ZERO;
    }

    PageEntry entry = p1->entries[pg_p1_index(&page)];
	//printf("Entry for index(%d), page(%d): %D\n",pg_p1_index(&page), page.number, entry);
    return pg_entry_pointed_frame(&entry, frame);

TABLE_ZERO:
    if(!p3) {
        return 0;
    }
    PageEntry *p3_entry = &p3->entries[pg_p3_index(&page)];
    // 1GiB page?
    struct Frame start_frame;
    if (pg_entry_pointed_frame(p3_entry, &start_frame)) {
        if (pg_entry_has_flags(p3_entry, EF_HUGE_PAGE)) {
            // address must be 1GiB aligned
            assert(start_frame.number % (ENTRY_COUNT * ENTRY_COUNT) == 0);
            frame->number = start_frame.number + pg_p2_index(&page) * ENTRY_COUNT +
                pg_p1_index(&page);
            return 1;
        }
    }
    if ((p2 = pg_table_next_table(p3, pg_p3_index(&page)))) {
        PageEntry *p2_entry = &p2->entries[pg_p2_index(&page)];
        // 2MiB page?
        if (pg_entry_pointed_frame(p2_entry, &start_frame)) {
            if (pg_entry_has_flags(p2_entry, EF_HUGE_PAGE)) {
                // address must be 2MiB aligned
                assert(start_frame.number % ENTRY_COUNT == 0);
                frame->number = start_frame.number + pg_p1_index(&page);
                return 1;
            }
        }
    }

    //printf("no page found :(\n");
    return 0;
}

uint32_t pg_p4_index(struct Page *page)
{
    return (page->number >> 27) & 0777U;
}

uint32_t pg_p3_index(struct Page *page)
{
    return (page->number >> 18) & 0777U;
}

uint32_t pg_p2_index(struct Page *page)
{
    return (page->number >> 9) & 0777U;
}

uint32_t pg_p1_index(struct Page *page)
{
    return (page->number >> 0) & 0777U;
}

void pg_map(struct Page *page, PageEntryFlags flags,
        struct AreaFrameAllocator *allocator)
{
    struct Frame f;
    fa_allocate_frame(allocator, &f);
    pg_mag_page_to_frame(page, &f, flags, allocator);
}

void pg_mag_page_to_frame(struct Page *page, struct Frame *frame,
        PageEntryFlags flags, struct AreaFrameAllocator *allocator)
{
    struct Table *p3 = pg_next_table_create(P4_TABLE, pg_p4_index(page), allocator);
    struct Table *p2 = pg_next_table_create(p3, pg_p3_index(page), allocator);
    struct Table *p1 = pg_next_table_create(p2, pg_p2_index(page), allocator);

    assert(pg_entry_is_unused(&p1->entries[pg_p1_index(page)]));
    pg_entry_set(&p1->entries[pg_p1_index(page)], frame, flags | EF_PRESENT);
}

void pg_identity_map(struct Frame *frame, PageEntryFlags flags,
        struct AreaFrameAllocator *allocator)
{
    struct Page page = pg_containing_address(frame_start_adress(frame));
    pg_mag_page_to_frame(&page, frame, flags, allocator);

}

static void pg_tlb_flush(uint64_t addr)
{
    asm volatile ("invlpg (%0)" : : "r"(addr) : "memory");
}

void pg_unmap(struct Page *page, struct AreaFrameAllocator *allocator)
{
    struct Table *p3 = pg_table_next_table(P4_TABLE, pg_p4_index(page));
    struct Table *p2 = pg_table_next_table(p3, pg_p3_index(page));
    struct Table *p1 = pg_table_next_table(p2, pg_p2_index(page));

    struct Frame f;
    int frame_exists = pg_entry_pointed_frame(&p1->entries[pg_p1_index(page)], &f);
    assert(frame_exists);
    pg_entry_set_unused(&p1->entries[pg_p1_index(page)]);
    // flush TLB:
    pg_tlb_flush(pg_start_address(page));
    //TODO: free p(1,2,3) table if empty
    fa_deallocate_frame(allocator, &f);
}

struct Table *pg_next_table_create(struct Table *self, tableindex index,
        struct AreaFrameAllocator *allocator)
{
    if (pg_table_next_table(self, index) == 0) {
        assert(!pg_entry_has_flags(&self->entries[index], EF_HUGE_PAGE));

        struct Frame frame;
        int available = fa_allocate_frame(allocator, &frame);
        assert(available);
        pg_entry_set(&self->entries[index], &frame, EF_PRESENT | EF_WRITABLE);
        pg_table_set_zero(pg_table_next_table(self, index));
    }

    return pg_table_next_table(self, index);
}
