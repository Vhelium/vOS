global long_mode_start

section .text
bits 64
long_mode_start:
    ; call the C main
    extern c_main
    call c_main

.os_returned:
    ; c main returned - print `OS returned`
	mov rax, 0x4f724f202f532f4f
    mov [0xb8000], rax
    mov rax, 0x4f724f754f744f65
    mov [0xb8008], rax
    mov rax, 0x4f214f644f654f6e
    mov [0xb8010], rax
    hlt
