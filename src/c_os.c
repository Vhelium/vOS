#include <stdint.h>
#include "vlib.h"
#include "vga_buffer.h"
#include "vio.h"
#include "multiboot2-elf64/multiboot2_elf64.h"
#include "multiboot2-elf64/memory_map.h"
#include "multiboot2-elf64/elf_sections.h"
#include "memory/frame.h"
#include "memory/area_frame_allocator.h"

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

    uint64_t multiboot_start = multiboot_information_address;
    uint64_t multiboot_end = multiboot_start + (bi->total_size);

    uint64_t kernel_start = -1;
    uint64_t kernel_end = -1;

    printf("kernel sections:\n");
    while(esIter.remaining_sections > 0) {
        enum ElfSectionType typ = esIter.current_section->typ;
        if (typ != Unused) {
            // check for min/max 
            if (kernel_start == -1) {
                kernel_start = esIter.current_section->addr;
                kernel_end = esIter.current_section->addr + 
                    esIter.current_section->size;
            }
            else {
                if (esIter.current_section->addr < kernel_start) {
                    // better start
                    kernel_start = esIter.current_section->addr;
                }
                if (esIter.current_section->addr +
                        esIter.current_section->size > kernel_end) {
                    // better end
                    kernel_end = esIter.current_section->addr +
                        esIter.current_section->size;
                }
            }

            // print it out
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

    printf("\n");
    

    // TODO: test area frame allocator
    struct AreaFrameAllocator frame_allocator = fa_create(kernel_start, kernel_end,
            multiboot_start, multiboot_end, mb_get_memory_areas(mmt));

    struct Frame f;
    while(fa_allocate_frame(&frame_allocator, &f)) {
        // keep allocating frames
        /* printint(f.number);
        printf(" ");
        i++; */
    }
    printf("allocated frames: ");
    printint(f.number);
    printf("\n");

    printf("g%d m%d", 8, 8);

    for(;;) {}
}

