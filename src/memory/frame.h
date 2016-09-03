#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

typedef uint64_t PhysicalAdress;
typedef uint64_t VirtualAdress;

#define PAGE_SIZE 4096U

struct Frame {
    uint32_t number;
};

struct FrameIter {
    struct Frame start;
    struct Frame end;
};

struct Frame frame_containing_address(uint64_t address);

PhysicalAdress frame_start_adress(struct Frame *self);

struct FrameIter frame_range_inclusive_iter(struct Frame start, struct Frame end);

/* returns 1 if next frame is written into `f`
 * returns 0 if no next frame exists */
int frame_range_inclusive_next(struct FrameIter *iter, struct Frame *f);

#endif
