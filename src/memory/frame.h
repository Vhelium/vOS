#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

#define PAGE_SIZE 4096

struct Frame {
    uint32_t number;
};

struct Frame frame_containing_address(uint64_t address);

#endif
