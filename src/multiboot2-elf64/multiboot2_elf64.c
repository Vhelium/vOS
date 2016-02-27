#include <stdint.h>
#include "../vlib.h"
#include "../vga_buffer.h"
#include "multiboot2_elf64.h"
#include "memory_map.h"

struct BootInformation *mb_load(int32_t adress)
{
    struct BootInformation *multiboot = (struct BootInformation *)adress;
    assert(mb_has_valid_end_tag(multiboot));
    return multiboot;
}

int mb_has_valid_end_tag(struct BootInformation *bi)
{
    struct Tag END_TAG = {0, 8};
    struct Tag end_tag = 
        *((struct Tag *) ((uintptr_t)bi + (bi->total_size - END_TAG.size)));

    return end_tag.typ == END_TAG.typ && end_tag.size == END_TAG.size;
}

struct Tag *mb_get_tag(struct BootInformation *bi, int typ)
{
    struct Tag *cur = &(bi->first_tag);
    while (!(cur->typ == 0 && cur->size == 8)) {
        if (cur->typ == typ) {
            return cur;
        }
        // calculate next tag
        uintptr_t cur_addr = (uintptr_t) cur;
        cur_addr += (uintptr_t) cur->size;
        cur_addr = ((cur_addr-1) & ~0x7) + 0x8; // align at 8 byte
        cur = (struct Tag *) cur_addr;
    }
    return 0;
}

struct MemoryMapTag *mb_get_memory_map_tag(struct BootInformation *bi)
{
    struct MemoryMapTag *mmt = (struct MemoryMapTag *) mb_get_tag(bi, 6);
    return mmt;
}
