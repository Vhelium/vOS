#include "paging.h"
#include "table.h"
#include "entry.h"
#include "../frame.h"
#include "../area_frame_allocator.h"
#include "../../vlib.h"
#include "../../vio.h"
#include "../../multiboot2-elf64/multiboot2_elf64.h"
#include "../../multiboot2-elf64/elf_sections.h"

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

// returns the Page which contains the given VirtualAdress
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

// initializes the physical `frame` corresponding to the provided page
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
    pg_map_page_to_frame(page, &f, flags, allocator);
}

void pg_map_page_to_frame(struct Page *page, struct Frame *frame,
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
    pg_map_page_to_frame(&page, frame, flags, allocator);

}

static void pg_tlb_flush(uint64_t addr)
{
    asm volatile ("invlpg (%0)" : : "r"(addr) : "memory");
}

static void pg_tlb_flush_all()
{
    asm volatile (
            "mov %%cr3,%%rax\n\t"
            "mov %%rax,%%cr3"
            : : : "memory");
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

struct InactivePageTable pg_inactive_pt_create(struct Frame *frame,
        struct Page *tmp_page, struct AreaFrameAllocator *allocator)
{
    // check if sth is already mapped here:
    struct Frame f;
    assert(pg_translate_page(&f, *tmp_page) == 0);

    // map temp page to page table frame in active table
    pg_map_page_to_frame(tmp_page, frame, EF_WRITABLE, allocator);

    // now it should be mapped
    assert(pg_translate_page(&f, *tmp_page) != 0);

    printf("next free frame: %d", allocator->next_free_frame.number);
    printf("\n");

    printf("frame: %d", f.number);
    printf("\n");
    //assert(0);

    struct Table *table = (struct Table *)pg_start_address(tmp_page);
    pg_table_set_zero(table);

    pg_entry_set(&table->entries[511], frame, EF_PRESENT | EF_WRITABLE);

    pg_unmap(tmp_page, allocator);

    return (struct InactivePageTable) { *frame };
}

static uint64_t get_cr3_content()
{
    uint64_t content;
    asm volatile ("mov %%cr3,%0"
        : "=r" (content)
        :
        :);
    return content;
}

static void set_cr3_content(uint64_t c)
{
    asm volatile ("mov %0,%%cr3"
        :
        : "r" (c)
        :);
}

static struct InactivePageTable pg_switch_tables(struct InactivePageTable *new_table)
{
    struct Frame p4_frame = frame_containing_address(get_cr3_content());
    struct InactivePageTable old_table = { p4_frame };

    set_cr3_content((uint64_t)frame_start_adress(&new_table->p4_frame));

    return old_table;
}

void pg_remap_kernel(struct AreaFrameAllocator *allocator, struct BootInformation *bi)
{
    // allocate active table
    // TODO

    struct Page tmp_page = { 0xdeadbeaf };
    struct Frame tmp_frame;
    // allocate frame for the new temporary page table
    int e = fa_allocate_frame(allocator, &tmp_frame);
    assert(e);

    struct InactivePageTable new_table =
        pg_inactive_pt_create(&tmp_frame, &tmp_page, allocator);

    struct Frame backup = frame_containing_address(get_cr3_content());

    // map tmp_page to current p4 table
    struct Frame frame_c1 = { backup.number };
    pg_map_page_to_frame(&tmp_page, &frame_c1, EF_WRITABLE, allocator);
    struct Table *p4_table = (struct Table *)pg_start_address(&tmp_page);

    // overwrite recursive mapping
    struct Frame frame_c2 = { new_table.p4_frame.number };
    pg_entry_set(&P4_TABLE->entries[511], &frame_c2, EF_PRESENT | EF_WRITABLE);
    pg_tlb_flush_all();

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // >>>>>> do stuff in new context
    
    // identity map the allocated kernel sections
    struct ElfSectionsTag *est = mb_get_elf_sections_tag(bi);
    struct ElfSectionIter esIter = mb_get_elf_sections(est);

    while(esIter.remaining_sections > 0) {
        struct ElfSection *section = esIter.current_section;
        if (!mb_section_is_allocated(section))
        {
            // section is not loaded to memory
            continue;
        }

        assert(section->addr % PAGE_SIZE == 0);
        printf("mapping section at addr: ");
        printlonghex(section->addr);
        printf(", size: ");
        printlonghex(section->size);
        printf("\n");

        uint64_t flags = pg_from_elf_section_flags(section);

        struct Frame start_frame =
            frame_containing_address(section->addr);
        struct Frame end_frame =
            frame_containing_address(section->addr + section->size - 1);

        struct FrameIter iter =
            frame_range_inclusive_iter(start_frame, end_frame);
        struct Frame f;
        while (frame_range_inclusive_next(&iter, &f) != 0) {
            pg_identity_map(&f, flags, allocator);
        }

        mb_get_next_elf_section(&esIter);
    }
    // identity map VGA text buffer
    struct Frame vga_buffer_frame = frame_containing_address(0xb8000);
    pg_identity_map(&vga_buffer_frame, EF_WRITABLE, allocator);
    
    // itendity map the multiboot info structure
    struct Frame multiboot_start =
        frame_containing_address((uint64_t)bi);
    struct Frame multiboot_end =
        frame_containing_address((uint64_t)bi + bi->total_size - 1);

    struct FrameIter iter =
        frame_range_inclusive_iter(multiboot_start, multiboot_end);
    struct Frame f;
    while (frame_range_inclusive_next(&iter, &f) != 0) {
        pg_identity_map(&f, EF_PRESENT, allocator);
    }
    
    // >>>>>> end context
    // >>>>>>>>>>>>>>>>>>
    
    // restore recursive mapping to original p4 table
    pg_entry_set(&p4_table->entries[511], &backup, EF_PRESENT | EF_WRITABLE);
    pg_tlb_flush_all();

    // unmap temporary page
    pg_unmap(&tmp_page, allocator);

    // switch in new table
    struct InactivePageTable old_table = pg_switch_tables(&new_table);

    printf("NEW TABLE ACTIVE!");

    // set up guard page (below stack, such that overflow = page fault)
    struct Page old_p4_page =
        pg_containing_address(frame_start_adress(&old_table.p4_frame));
    pg_unmap(&old_p4_page, allocator);
    printf("guard page at %x", pg_start_address(&old_p4_page));
}
