#include "elf_sections.h"

const uint64_t ELF_SECTION_WRITABLE = 0x1;
const uint64_t ELF_SECTION_ALLOCATED = 0x2;
const uint64_t ELF_SECTION_EXECUTABLE = 0x4;

struct ElfSectionIter mb_get_elf_sections(struct ElfSectionsTag *est)
{
    struct ElfSectionIter iter;
    iter.current_section = &est->first_section;
    iter.remaining_sections = est->number_of_sections -1;
    iter.entry_size = est->entry_size;

    return iter;
}

void mb_get_next_elf_section(struct ElfSectionIter *iter)
{
    if (iter->remaining_sections <= 0)
        return;
    
    uintptr_t next_section_addr =
        ((uintptr_t) iter->current_section) + iter->entry_size;
    iter->current_section = (struct ElfSection *) next_section_addr;
    iter->remaining_sections -= 1;
}

int mb_section_contains_flag(struct ElfSection *s, uint64_t flag)
{
    return (s->flags & flag) != 0;
}

int mb_section_is_allocated(struct ElfSection *s)
{
    return mb_section_contains_flag(s, ELF_SECTION_ALLOCATED);
}
