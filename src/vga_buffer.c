#include "vga_buffer.h"
#include "vlib.h"

struct Buffer *vga_writer_get_buffer(struct Writer*);
void vga_writer_new_line(struct Writer *);
void vga_writer_clear_row(struct Writer *writer, size_t row);

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

void vga_writer_write_str(struct Writer *writer, const char *str)
{
    while (*str) {
        vga_writer_write_byte(writer, *str++);
    }
}

void vga_writer_write_int(struct Writer *writer, int i)
{
    if (i >= 10) {
        vga_writer_write_int(writer, i / 10);
    }
    vga_writer_write_byte(writer, (char) (i%10 + 48));
}

void vga_writer_write_hex(struct Writer *writer, int h)
{
    if (h >= 16) {
        vga_writer_write_hex(writer, h / 16);
    }
    char digit = h % 16;
    vga_writer_write_byte(writer, digit < 10 ? (digit+ 48) : (digit + 55));
}

void vga_writer_new_line(struct Writer *writer)
{
    int row, i;
    for (row = 0; row < BUFFER_HEIGHT-1; ++row) {
        struct Buffer *buffer = vga_writer_get_buffer(writer);
        for(i = 0; i < BUFFER_WIDTH; ++i)
            buffer->chars[row][i] = buffer->chars[row+1][i];
    }
    vga_writer_clear_row(writer, BUFFER_HEIGHT-1);
    writer->column_position = 0;
}

void vga_writer_clear_row(struct Writer *writer, size_t row)
{
    if (row < 0 || row >= BUFFER_HEIGHT)
        return;

    int i;
    struct ScreenChar blank;
    blank.ascii_character = ' ';
    blank.color_code = writer->color_code;
    
    // Set whole line to `blank`
    struct Buffer *buffer = vga_writer_get_buffer(writer);
    for(i = 0; i < BUFFER_WIDTH; buffer->chars[row][i++] = blank);
}

void vga_writer_clear_screen(struct Writer *writer)
{
    int i;
    for (i = 0; i < BUFFER_HEIGHT; ++i) {
        vga_writer_clear_row(writer, i);
    }
    writer->column_position = 0;
}

/* returns a Writer instance for output.
 * Thread safe. */
static volatile int vga_writer_lock = 0;
struct Writer *vga_writer_get_instance()
{
    while (__sync_lock_test_and_set(&vga_writer_lock, 1));

    static struct Writer instance;
    static int init = 0;

    if (!init) {
        instance.column_position = 0;
        instance.color_code = vga_get_color_code((char) LightGreen, (char) Black);
        instance.buffer = (struct Buffer*) 0xb8000;
        init = 1;
    }

    __sync_lock_release(&vga_writer_lock);

    return &instance;
}
