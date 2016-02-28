#ifndef ELF_SECTIONS_H
#define ELF_SECTIONS_H

#include <stdint.h>

struct ElfSection {
    int32_t name;
    int32_t typ;
    int64_t flags;
    int64_t addr;
    int64_t offset;
    int64_t size;
    int32_t link;
    int32_t info;
    int64_t addralign;
    int64_t entry_size;
};

struct ElfSectionsTag {
    int32_t typ;
    int32_t size;
    int32_t number_of_sections;
    int32_t entry_size;
    int32_t shndx; // string table
    struct ElfSection first_section;
};

struct ElfSectionIter {
    struct ElfSection *current_section;
    int32_t remaining_sections;
    int32_t entry_size;
};

enum ElfSectionType {
	Unused = 0,
    ProgramSection = 1,
    LinkerSymbolTable = 2,
    StringTable = 3,
    RelaRelocation = 4,
    SymbolHashTable = 5,
    DynamicLinkingTable = 6,
    Note = 7,
    Uninitialized = 8,
    RelRelocation = 9,
    Reserved = 10,
    DynamicLoaderSymbolTable = 11
};

/* returns Iterator for ElfSection*/
struct ElfSectionIter mb_get_elf_sections(struct ElfSectionsTag *est);

/* go to next item in Iterator `iter` */
void mb_get_next_elf_section(struct ElfSectionIter *iter);

#endif
