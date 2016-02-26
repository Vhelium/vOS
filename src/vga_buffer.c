#include "vga_buffer.h"
#include "vlib.h"

struct Buffer *vga_writer_get_buffer(struct Writer*);
void vga_writer_new_line(struct Writer *);

ColorCode vga_get_color_code(enum Color foreground, enum Color background)
{
	return (background << 4) | (foreground);
}

void vga_writer_write_byte(struct Writer *writer, char byte)
{
    if (byte == '\n') {
        vga_writer_new_line(writer);
    }
    else {
        if (writer->column_position >= BUFFER_WIDTH) {
            vga_writer_new_line(writer);
        }

        size_t row = BUFFER_HEIGHT - 1;
        size_t col = writer->column_position;

        struct ScreenChar sc;
        sc.ascii_character = byte;
        sc.color_code = writer->color_code;

        vga_writer_get_buffer(writer)->chars[row][col] = sc;
        writer->column_position += 1;
    }
}

struct Buffer *vga_writer_get_buffer(struct Writer *writer)
{
    return writer->buffer;
}

void vga_writer_write_str(struct Writer *writer, char *str)
{
    while (*str) {
        vga_writer_write_byte(writer, *str++);
    }
}

void vga_writer_new_line(struct Writer *writer)
{
    /* TODO */
}
