section .multiboot_header
header_start:
    dd 0xE85250D6                   ; magic number (multiboot 2)
    dd 0                            ; architecture 0 (protected mode i386)
    dd header_end - header_start    ; header length
    dd 0x100000000 - (0xE85250D6 + 0x0 + header_end - header_start)  ; checksum

    ; insert optional multiboot tags here (and adjust above)

    ; end flag
    dw 0x0  ; type
    dw 0x0  ; flags
    dd 0x8  ; size

header_end:
