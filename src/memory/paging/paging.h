#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "../frame.h"

#define ENTRY_COUNT 512

struct Page {
    uint32_t number;
};

#endif
