#include "memory_map.h"
#include <stdint.h>

struct MemoryAreaIter mb_get_memory_areas(struct MemoryMapTag *mmt)
{
    struct MemoryAreaIter iter;
    iter.current_area = &mmt->first_area;
    iter.last_area = (struct MemoryArea *)
        (((uintptr_t) mmt) + mmt->size - mmt->entry_size);
    iter.entry_size = mmt->entry_size;
    
    return iter;
}

/* returns:
 *      1 - valid RAM area
 *      0 - invalid RAM area
 *     -1 - reached end of list
 */
int mb_get_next_memory_area(struct MemoryAreaIter *iter)
{
    iter->current_area = (struct MemoryArea *)
        (((uintptr_t) iter->current_area) + iter->entry_size);

    if (iter->current_area > iter->last_area) {
        return -1; // end of list
    }
    if (iter->current_area->typ == 1) {
        return 1; // valid area
    }
    else
        return 0; // invalid area
}
