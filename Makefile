.RECIPEPREFIX := >

CC      := clang
LD      := ld.lld
CFLAGS  := --target=x86_64-unknown-elf -std=gnu11 -ffreestanding \
           -fno-stack-protector -fno-stack-check -fno-PIC -fno-pie \
           -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 \
           -mno-red-zone -mcmodel=kernel -mgeneral-regs-only -O2 -Wall -Wextra -Iinclude

LDFLAGS := -m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 -T linker.ld

EXAMPLE ?= hello
SRC     := $(wildcard src/*.c) examples/$(EXAMPLE)/main.c
OBJ     := $(patsubst %.c,build/%.o,$(SRC))
LIMINE  := /usr/share/limine
OVMF    ?= /usr/share/edk2/x64/OVMF.4m.fd

all: build/core.iso

build/%.o: %.c
> @mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

build/kernel: $(OBJ) linker.ld
> $(LD) $(LDFLAGS) $(OBJ) -o $@

build/core.iso: build/kernel limine.conf
> rm -rf build/iso_root
> mkdir -p build/iso_root/boot/limine build/iso_root/EFI/BOOT
> cp build/kernel build/iso_root/boot/kernel
> cp limine.conf $(LIMINE)/limine-bios.sys $(LIMINE)/limine-bios-cd.bin \
>    $(LIMINE)/limine-uefi-cd.bin build/iso_root/boot/limine/
> cp $(LIMINE)/BOOTX64.EFI build/iso_root/EFI/BOOT/
> xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
>   -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
>   -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
>   -efi-boot-part --efi-boot-image --protective-msdos-label \
>   build/iso_root -o $@
> limine bios-install $@

run: build/core.iso
> qemu-system-x86_64 -cdrom $< -m 256M

run-uefi: build/core.iso
> qemu-system-x86_64 -bios $(OVMF) -cdrom $< -m 256M

clean:
> rm -rf build

.PHONY: all run run-uefi clean
.DELETE_ON_ERROR:

