#include <stdint.h>
#include "vlib.h"
#include "vga_buffer.h"
#include "multiboot2-elf64/multiboot2_elf64.h"
#include "multiboot2-elf64/memory_map.h"
#include "multiboot2-elf64/elf_sections.h"

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
    struct MemoryAreaIter mmIter = mb_get_memory_areas(mmt);

    int valid = mmIter.current_area->typ == 1;
    do {
        if (valid) {
            printf("    start: ");
            printhex(mmIter.current_area->base_addr);
            printf(", length: ");
            printhex(mmIter.current_area->length);
            printf("\n");
        }
        else {
            // printf("invalid area\n");
        }
    } while ((valid = mb_get_next_memory_area(&mmIter)) != -1);

    struct ElfSectionsTag *est = mb_get_elf_sections_tag(bi);
    struct ElfSectionIter esIter = mb_get_elf_sections(est);

    printf("kernel sections:\n");
    while(esIter.remaining_sections > 0) {
        enum ElfSectionType typ = esIter.current_section->typ;
        if (typ != Unused) {
            printf("    addr: ");
            printlonghex(esIter.current_section->addr);
            printf(", size: ");
            printlonghex(esIter.current_section->size);
            printf(", flags: ");
            printlonghex(esIter.current_section->flags);
            printf("\n");
        }
        else {
            // printf("unused area\n");
        }

        mb_get_next_elf_section(&esIter);
    }

    for(;;) {}
}

