#include "../vlib.h"
#include "../vga_buffer.h"
#include "multiboot2_elf64.h"

struct BootInformation *mb_load(int32_t adress)
{
    struct BootInformation *multiboot = (struct BootInformation *)adress;
    //TODO: assert(multiboot->has_valid_end_tag());
    return multiboot;
}

int mb_has_valid_end_tag(struct BootInformation *bi)
{
    struct Tag END_TAG = {0, 8};
    struct Tag end_tag = 
        *((struct Tag *) ((int32_t)bi + (bi->total_size - END_TAG.size)));

    return end_tag.typ == END_TAG.typ && end_tag.size == END_TAG.size;
}
