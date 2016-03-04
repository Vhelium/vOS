#include <stdint.h>
#include "vlib.h"
#include "vga_buffer.h"
#include "vio.h"
#include "multiboot2-elf64/multiboot2_elf64.h"
#include "multiboot2-elf64/memory_map.h"
#include "multiboot2-elf64/elf_sections.h"
#include "memory/frame.h"
#include "memory/area_frame_allocator.h"
#include "memory/paging/paging.h"

void c_main(uint32_t multiboot_information_address)
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
    
    struct AreaFrameAllocator frame_allocator = fa_create(kernel_start, kernel_end,
            multiboot_start, multiboot_end, mb_get_memory_areas(mmt));

    /*
    // allocation test
    struct Frame f;
    while(fa_allocate_frame(&frame_allocator, &f)) { }
    printf("allocated frames: ");
    printint(f.number);
    printf("\n\n\n");
    */
	
    /*
    // translation test
    printf("0: %D\n", pg_translate(0x0));
    printf("4096: %D\n", pg_translate(4096));
    printf("512*4096: %D\n", pg_translate(512*4096));
    printf("300*512*4096: %D\n", pg_translate(300*512*4096));
    printf("512*512*4096: %D\n", pg_translate(512*512*4096));
    printf("512*512*4096-1: %D\n", pg_translate(512*512*4096-1));
    */

    // map a page
	VirtualAdress addr = 42U * 512U * 512U * 4096U; // 42th P3 entry
    struct Page page = pg_containing_address(addr);
    struct Frame frame;
    printf("vadress: %l\n\n", addr);
    fa_allocate_frame(&frame_allocator, &frame);
    printf("None = %l, map to Frame %d\n", pg_translate(addr), frame.number);
    pg_mag_page_to_frame(&page, &frame, 0, &frame_allocator);
    printf("Some = %l\n", pg_translate(addr));
    fa_allocate_frame(&frame_allocator, &frame);
    printf("next free frame: %d\n", frame.number);

    // print out what the mapped page contains
    uint64_t *content = (uint64_t *) pg_start_address(&page);
    printf("content: %l\n", *content);

    pg_unmap(&page, &frame_allocator);
    printf("None = %l\n", pg_translate(addr));

    /*
    // try print content at that adress (will PageFault since unmapped)
    content = (uint64_t *) pg_start_address(&page);
    printf("content: %l\n", *content);
    */
	
    for(;;) {}
}
