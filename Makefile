.RECIPEPREFIX := >

CC      := clang
LD      := ld.lld
CFLAGS  := --target=x86_64-unknown-elf -std=gnu11 -ffreestanding \
           -fno-stack-protector -fno-stack-check -fno-PIC -fno-pie \
           -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 \
           -mno-red-zone -mcmodel=kernel -mgeneral-regs-only -O2 -Wall -Wextra -Iinclude \
           -MMD -MP

LDFLAGS := -m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 -T linker.ld

EXAMPLE ?= hello
BUILD   := build/$(EXAMPLE)
SRC     := $(wildcard src/*.c) examples/$(EXAMPLE)/main.c
OBJ     := $(patsubst %.c,$(BUILD)/%.o,$(SRC))
LIMINE  := /usr/share/limine
OVMF    ?= /usr/share/edk2/x64/OVMF.4m.fd

all: $(BUILD)/core.iso

$(BUILD)/%.o: %.c
> @mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel: $(OBJ) linker.ld
> $(LD) $(LDFLAGS) $(OBJ) -o $@

$(BUILD)/core.iso: $(BUILD)/kernel limine.conf
> rm -rf $(BUILD)/iso_root
> mkdir -p $(BUILD)/iso_root/boot/limine $(BUILD)/iso_root/EFI/BOOT
> cp $(BUILD)/kernel $(BUILD)/iso_root/boot/kernel
> cp limine.conf $(LIMINE)/limine-bios.sys $(LIMINE)/limine-bios-cd.bin \
>    $(LIMINE)/limine-uefi-cd.bin $(BUILD)/iso_root/boot/limine/
> cp $(LIMINE)/BOOTX64.EFI $(BUILD)/iso_root/EFI/BOOT/
> xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
>   -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
>   -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
>   -efi-boot-part --efi-boot-image --protective-msdos-label \
>   $(BUILD)/iso_root -o $@
> limine bios-install $@

run: $(BUILD)/core.iso
> qemu-system-x86_64 -cdrom $< -m 256M

run-uefi: $(BUILD)/core.iso
> qemu-system-x86_64 -bios $(OVMF) -cdrom $< -m 256M

clean:
> rm -rf build

.PHONY: all run run-uefi clean
.DELETE_ON_ERROR:

-include $(OBJ:.o=.d)
