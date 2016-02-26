#ifndef VGA_BUFFER_H
#define VGA_BUFFER_H

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

void vga_writer_write_str(struct Writer *writer, char *str);

#endif
