#ifndef ENTRY_H
#define ENTRY_H

#include "../frame.h"

/* if Entry is == 0, the Entry counts as unused */

typedef uint64_t Entry;
typedef uint64_t EntryFlags;

enum EntryFlag {
    PRESENT = 1 << 0,
    WRITABLE = 1 << 1,
    USER_ACCESSIBLE = 1 << 2,
    WRITE_THROUGH = 1 << 3,
    NO_CACHE = 1 << 4,
    ACCESSED = 1 << 5,
    DIRTY = 1 << 6,
    HUGE_PAGE = 1 << 7,
    GLOBAL = 1 << 8,
    NO_EXECUTE = 1 << 63
}

int pg_entry_is_unused(Entry *entry);

void pg_entry_set_unused(Entry *entry);

int pg_entry_has_flags(Entry *self, EntryFlags flags);

int pg_entry_set_flags(Entry *self, EntryFlags flags);

/* returns 1 if a frame was found and written to `frame`
 * returns 0 if no such frame was found */
int pg_entry_pointed_frame(Entry *self, Frame *frame);

#endif
