arch ?= x86_64
target ?= $(arch)-unknown-linux-gnu
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
c_source_files := $(wildcard src/*.c)
c_object_files := $(patsubst src/%.c, \
	build/target/$(target)/%.o, $(c_source_files))
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))
mb_source_files := $(wildcard src/multiboot2-elf64/*.c)
mb_object_files := $(patsubst src/multiboot2-elf64/%.c, \
	build/target/$(target)/multiboot2-elf64/%.o, $(mb_source_files))

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(linker_script) $(c_object_files) $(mb_object_files)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files) $(mb_object_files)

# compile C files
build/target/$(target)/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@gcc -Wall -g -c $< -o $@

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

# compile multiboot files
build/target/$(target)/multiboot2-elf64/%.o: src/multiboot2-elf64/%.c
	@mkdir -p $(shell dirname $@)
	@gcc -Wall -g -c $< -o $@
