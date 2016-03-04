#include <stdint.h>
#include "table.h"
#include "entry.h"
#include "../../vlib.h"
#include "../../vio.h"

struct Table *P4_TABLE = (struct Table *) 0xfffffffffffff000;

void pg_table_set_zero(struct Table *self)
{
    int i;
    for (i=0; i < ENTRY_COUNT; ++i) {
        self->entries[i] = 0;
    }
}

uint64_t pg_table_next_table_address(struct Table *self, tableindex index)
{
    assert(index < ENTRY_COUNT && index >= 0);

    PageEntry entry = self->entries[index];
    if (pg_entry_has_flags(&entry, EF_PRESENT) &&
            !pg_entry_has_flags(&entry, EF_HUGE_PAGE)) {
        uint64_t table_adress = (uint64_t) self;
        return (table_adress << 9) | (index << 12);
    }
    else {
        return 0;
    }
}

struct Table *pg_table_next_table(struct Table *self, tableindex index)
{
    return (struct Table *) pg_table_next_table_address(self, index);
}
