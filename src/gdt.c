#include <stdint.h>

struct gdt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static uint64_t gdt[3] =
{
    0x0000000000000000,
    0x00AF9A000000FFFF,
    0x00CF92000000FFFF,
};

static struct gdt_ptr gdtr;

void gdt_init(void)
{
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    __asm__ volatile (
        "lgdt %0\n"
        "mov $0x10, %%ax\n" 
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushq $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        : : "m"(gdtr) : "rax", "memory"
    );
}