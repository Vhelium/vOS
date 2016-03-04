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

#mem_source_files := $(shell find src/memory -name '*.c')
#mem_object_files := $(addprefix build/target/$(target)/memory/, $(mem_source_files:src/memory/%.c=%.o))

mem_source_files := $(wildcard src/memory/*.c)
mem_object_files := $(patsubst src/memory/%.c, \
	build/target/$(target)/memory/%.o, $(mem_source_files))

mem_source_files2 := $(wildcard src/memory/paging/*.c)
mem_object_files2 := $(patsubst src/memory/paging/%.c, \
	build/target/$(target)/memory/paging/%.o, $(mem_source_files2))

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso) -m 500M

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles


$(kernel): $(assembly_object_files) $(linker_script) $(c_object_files) $(mb_object_files) $(mem_object_files) $(mem_object_files2)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files) $(mb_object_files) $(mem_object_files) $(mem_object_files2)

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
	
# compile memory files
#$(mem_object_files): $(mem_source_files)
build/target/$(target)/memory/%.o: src/memory/%.c
	@mkdir -p $(shell dirname $@)
	@gcc -Wall -g -c $< -o $@

build/target/$(target)/memory/paging/%.o: src/memory/paging/%.c
	@mkdir -p $(shell dirname $@)
	@gcc -Wall -g -c $< -o $@
