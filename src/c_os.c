#include <stdint.h>
#include "vlib.h"
#include "vga_buffer.h"
#include "multiboot2-elf64/multiboot2_elf64.h"
#include "multiboot2-elf64/memory_map.h"

void c_main(int32_t multiboot_information_address)
{
    printf("Kappa\nKeepo");

    cls();

    struct BootInformation *bi = mb_load(multiboot_information_address);
    int iv = mb_has_valid_end_tag(bi);

    printf("\nis end tag valid: ");
    vga_writer_write_int(WRITER, iv);
    printf("\n");

    printf("memory areas:\n");
    struct MemoryMapTag *mmt = mb_get_memory_map_tag(bi);
    struct MemoryAreaIter iter = mb_get_memory_areas(mmt);

    int valid = iter.current_area->typ == 1;
    do {
        if (valid) {
            printf("    start: ");
            printhex(iter.current_area->base_addr);
            printf(", length: ");
            printhex(iter.current_area->length);
            printf("\n");
        }
        else {
            // printf("invalid area\n");
        }
    } while ((valid = mb_get_next_memory_area(&iter)) != -1);

    for(;;) {}
}

