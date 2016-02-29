#include <stdint.h>
#include <stdarg.h>
#include "vio.h"
#include "vga_buffer.h"

void v_printf(const char *str, ... )
{
    va_list a_list;
    va_start(a_list, str);
    // loop all characters
    // if we find a %s, parse the following char and call corresp. funct
    while (*str) {
        if (*str == '%') {
            // format
            char format  = *(++str);
            if (format == 'd') {
                // print int
                printint(va_arg(a_list, int));
            }
            else if (format == 'x') {
                // print int as hex
                printhex(va_arg(a_list, int));
            }
            else if (format == 's') {
                // print string
                printstr(va_arg(a_list, char*));
            }
            else if (format == 'l') {
                // print long
                printlonghex(va_arg(a_list, long));
            }
            str++;
        }
        else {
            // normal character
            vga_writer_write_byte(WRITER, *str++);
        }
    }
}
