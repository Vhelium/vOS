#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

typedef uint64_t PhysicalAdress;
typedef uint64_t VirtualAdress;

#define PAGE_SIZE 4096U

struct Frame {
    uint32_t number;
};

struct Frame frame_containing_address(uint64_t address);

PhysicalAdress frame_start_adress(struct Frame *self);

#endif
