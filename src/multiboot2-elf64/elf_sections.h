#ifndef ELF_SECTIONS_H
#define ELF_SECTIONS_H

#include <stdint.h>

struct ElfSection {
    uint32_t name;
    uint32_t typ;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entry_size;
} __attribute__((packed));

struct ElfSectionsTag {
    uint32_t typ;
    uint32_t size;
    uint32_t number_of_sections;
    uint32_t entry_size;
    uint32_t shndx; // string table
    struct ElfSection first_section;
} __attribute__((packed));

struct ElfSectionIter {
    struct ElfSection *current_section;
    uint32_t remaining_sections;
    uint32_t entry_size;
};

extern const uint64_t ELF_SECTION_WRITABLE;
extern const uint64_t ELF_SECTION_ALLOCATED;
extern const uint64_t ELF_SECTION_EXECUTABLE;

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

int mb_section_contains_flag(struct ElfSection *s, uint64_t flag);

int mb_section_is_allocated(struct ElfSection *s);

#endif
