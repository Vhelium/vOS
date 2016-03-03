#include "../frame.h"
#include "paging.h"

int pg_entry_is_unused(Entry *entry)
{
    return *entry == 0;
}

void pg_entry_set_unused(Entry *entry)
{
    *entry = 0;
}

int pg_entry_has_flags(Entry *self, EntryFlags flags)
{
    return *self & flags == flags;
}

int pg_entry_set_flags(Entry *self, EntryFlags flags)
{
    *self = *self | flags;
}

int pg_entry_pointed_frame(Entry *self, struct Frame *frame)
{
    if (pg_entry_has_flags((enum EntryFlags) PRESENT)) {
        *frame = frame_containing_address(*self & 0x000fffff_fffff000);
        return 1;
    }
    else {
        return 0;
    }
}
