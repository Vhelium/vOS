#include "vga_buffer.h"

void foo()
{
    struct Writer writer;
    writer.column_position = 0;
    writer.color_code = vga_get_color_code((char) LightGreen, (char) Black);
    writer.buffer = (struct Buffer*) 0xb8000;

    vga_writer_write_str(&writer, "Hello World");
}

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

    foo();

    for(;;) {}
}

