#include <stdint.h>
#include "frame.h"

struct Frame frame_containing_address(uint64_t address)
{
    struct Frame f = { (address / PAGE_SIZE) };
    return f;
}

PhysicalAdress frame_start_adress(struct Frame *self)
{
    return self->number * PAGE_SIZE;
}

struct FrameIter frame_range_inclusive_iter(struct Frame start, struct Frame end)
{
    return (struct FrameIter){ start, end };
}

int frame_range_inclusive_next(struct FrameIter *iter, struct Frame *f)
{
    if (iter->start.number <= iter->end.number) {
        *f = (struct Frame) { iter->start.number };
        iter->start.number += 1;
        return 1;
    }
    else
        return 0;
}
