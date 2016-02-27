#ifndef VLIB_H
#define VLIB_H

typedef unsigned int size_t;

#include "vga_buffer.h"

void __assert_fail(const char *__assertion, const char *__file,
	unsigned int __line, const char *__function);

# define __ASSERT_VOID_CAST (void)

# define assert(expr) \
  ((expr) \
   ? __ASSERT_VOID_CAST (0) \
   : __assert_fail (#expr, __FILE__, __LINE__, __func__))

#endif
