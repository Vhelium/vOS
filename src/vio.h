#ifndef VIO_H
#define VIO_H

#include <stdint.h>
#include "vga_buffer.h"

void v_printf(const char *str, ... );

#define printf(str, ...) v_printf (str, ##__VA_ARGS__)

#endif
