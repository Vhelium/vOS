#ifndef ENTRY_H
#define ENTRY_H

#include "../frame.h"
#include "../../multiboot2-elf64/elf_sections.h"

/* if Entry is == 0, the Entry counts as unused */

typedef uint64_t PageEntry;
typedef uint64_t PageEntryFlag;
typedef uint64_t PageEntryFlags;

extern const PageEntryFlag EF_PRESENT;
extern const PageEntryFlag EF_WRITABLE;
extern const PageEntryFlag EF_USER_ACCESSIBLE;
extern const PageEntryFlag EF_WRITE_THROUGH;
extern const PageEntryFlag EF_NO_CACHE;
extern const PageEntryFlag EF_ACCESSED;
extern const PageEntryFlag EF_DIRTY;
extern const PageEntryFlag EF_HUGE_PAGE;
extern const PageEntryFlag EF_GLOBAL;
extern const PageEntryFlag EF_NO_EXECUTE;

int pg_entry_is_unused(PageEntry *entry);

void pg_entry_set_unused(PageEntry *entry);

int pg_entry_has_flags(PageEntry *self, PageEntryFlags flags);

void pg_entry_set_flags(PageEntry *self, PageEntryFlags flags);

void pg_entry_set(PageEntry *self, struct Frame *frame, PageEntryFlags flags);

/* returns 1 if a frame was found and written to `frame`
 * returns 0 if no such frame was found */
int pg_entry_pointed_frame(PageEntry *self, struct Frame *frame);

PageEntryFlags pg_from_elf_section_flags(struct ElfSection *s);

#endif
