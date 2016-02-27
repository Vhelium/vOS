#include "vga_buffer.h"

void c_main(void)
{
	/*
    int i;
    char hello[] = "Hello World!";
	char color_byte = 0x9f; // white foreground, blue background

    // set up colored string
	char hello_colored[24];
    for (i = 0; i < 12; i++) {
        char *char_byte = hello + i;
        hello_colored[i*2] = *char_byte;
        hello_colored[i*2+1] = color_byte;
    }

    // write to buffer
    char *buffer_ptr = (char *)(0xb8000 + 1988);
    for (i = 0; i < 12; i++) {
        *buffer_ptr++ = hello_colored[i*2];
        *buffer_ptr++ = hello_colored[i*2+1];
    }
    */

    printf("Kappa\nKeepo");
    cls();
    printf("much wow");

    for(;;) {}
}

