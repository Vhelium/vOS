#include <stdint.h>
#include "vlib.h"
#include "vga_buffer.h"
#include "multiboot2-elf64/multiboot2_elf64.h"

void c_main(int32_t multiboot_information_address)
{
    printf("Kappa\nKeepo");

    cls();

    printf("Adress: ");
    printint((int) multiboot_information_address);
    printf("\n");

    struct BootInformation *bi = mb_load(multiboot_information_address);
    printf("BootInformation.totalSize: ");
    printint(bi->total_size);
    printf("\n");

    int iv = mb_has_valid_end_tag(bi);

    printf("\nis end tag valid: ");
    vga_writer_write_int(WRITER, iv);
    printf("\n");

    for(;;) {}
}

