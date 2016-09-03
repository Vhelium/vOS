#include "../../vlib.h"
#include "entry.h"
#include "../frame.h"
#include "paging.h"

const PageEntryFlag EF_PRESENT = 1 << 0;
const PageEntryFlag EF_WRITABLE = 1 << 1;
const PageEntryFlag EF_USER_ACCESSIBLE = 1 << 2;
const PageEntryFlag EF_WRITE_THROUGH = 1 << 3;
const PageEntryFlag EF_NO_CACHE = 1 << 4;
const PageEntryFlag EF_ACCESSED = 1 << 5;
const PageEntryFlag EF_DIRTY = 1 << 6;
const PageEntryFlag EF_HUGE_PAGE = 1 << 7;
const PageEntryFlag EF_GLOBAL = 1 << 8;
const PageEntryFlag EF_NO_EXECUTE = 0x80000000;

int pg_entry_is_unused(PageEntry *entry)
{
    return *entry == 0;
}

void pg_entry_set_unused(PageEntry *entry)
{
    *entry = 0;
}

int pg_entry_has_flags(PageEntry *self, PageEntryFlags flags)
{
    return (*self & flags) == flags;
}

void pg_entry_set_flags(PageEntry *self, PageEntryFlags flags)
{
    *self = *self | flags;
}

void pg_entry_set(PageEntry *self, struct Frame *frame, PageEntryFlags flags)
{
    assert((frame_start_adress(frame) & (~0x000ffffffffff000U)) == 0);
    *self = frame_start_adress(frame) | flags;
}

int pg_entry_pointed_frame(PageEntry *self, struct Frame *frame)
{
    if (pg_entry_has_flags(self, EF_PRESENT)) {
        *frame = frame_containing_address(*self & 0x000ffffffffff000U);
        return 1;
    }
    else {
        return 0;
    }
}

PageEntryFlags pg_from_elf_section_flags(struct ElfSection *s)
{
    PageEntryFlags flags = 0;
    if (mb_section_contains_flag(s, ELF_SECTION_ALLOCATED)) {
        flags = flags | EF_PRESENT;
    }
    if (mb_section_contains_flag(s, ELF_SECTION_WRITABLE)) {
        flags = flags | EF_WRITABLE;
    }
    if (!mb_section_contains_flag(s, ELF_SECTION_EXECUTABLE)) {
        flags = flags | EF_NO_EXECUTE;
    }
    return flags;
}
