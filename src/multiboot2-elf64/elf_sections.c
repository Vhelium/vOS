#include "elf_sections.h"

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
