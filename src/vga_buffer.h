#ifndef VGA_BUFFER_H
#define VGA_BUFFER_H

#include <stdint.h>
#include "vlib.h"

enum Color {
    Black      = 0,
    Blue       = 1,
    Green      = 2,
    Cyan       = 3,
    Red        = 4,
    Magenta    = 5,
    Brown      = 6,
    LightGray  = 7,
    DarkGray   = 8,
    LightBlue  = 9,
    LightGreen = 10,
    LightCyan  = 11,
    LightRed   = 12,
    Pink       = 13,
    Yellow     = 14,
    White      = 15
};

typedef char ColorCode;

ColorCode vga_get_color_code(enum Color, enum Color);

struct ScreenChar {
    char ascii_character;
    ColorCode color_code;
};

#define BUFFER_HEIGHT 25
#define BUFFER_WIDTH 80

struct Buffer {
    struct ScreenChar chars[BUFFER_HEIGHT][BUFFER_WIDTH];
};

struct Writer {
    size_t column_position;
    ColorCode color_code;
    struct Buffer *buffer;
};

void vga_writer_write_byte(struct Writer *writer, char byte);

void vga_writer_write_str(struct Writer *writer, const char *str);

void vga_writer_write_int(struct Writer *writer, uint32_t i);

void vga_writer_write_long_int(struct Writer *writer, uint64_t i);

void vga_writer_write_hex(struct Writer *writer, uint32_t h);

void vga_writer_write_long_hex(struct Writer *writer, uint64_t h);

void vga_writer_clear_screen(struct Writer *writer);

struct Writer *vga_writer_get_instance();

#define WRITER vga_writer_get_instance()

#define printstr(str) vga_writer_write_str(WRITER, str)

#define printint(i) vga_writer_write_int(WRITER, i)

#define printlongint(i) vga_writer_write_long_int(WRITER, i)

#define printhex(h) vga_writer_write_hex(WRITER, h)

#define printlonghex(h) vga_writer_write_long_hex(WRITER, h)

#define cls() vga_writer_clear_screen(WRITER)

#endif
