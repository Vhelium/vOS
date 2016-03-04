#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "entry.h"

#define ENTRY_COUNT 512

extern struct Table *P4_TABLE;

typedef uint32_t tableindex;

struct Table {
    PageEntry entries[ENTRY_COUNT];
};

struct Table pg_table_create();

void pg_table_set_zero(struct Table *self);

uint64_t pg_table_next_table_address(struct Table *self, tableindex index);

struct Table *pg_table_next_table(struct Table *self, tableindex index);

#endif
