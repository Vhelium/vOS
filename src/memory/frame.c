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
