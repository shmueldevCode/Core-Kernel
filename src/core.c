#include <stdint.h>
#include <stddef.h>
#include "kore.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[3] = {
    0xf9562b2d5c95a6c8, 0x6a7b384944536bdc, 3
};

void gdt_init(void);
void idt_init(void);

static void kernel_init(void){
    gdt_init();
    idt_init();
}

void screen_init(void);

KoreCreate Create = {
    .Kernel = kernel_init,
    .Screen = screen_init,
};

void *memset(void *d, int c, size_t n) {
    uint8_t *p =d;
    while (n--) *p++ = (uint8_t)c;
    return d;
}

void *memcpy(void *d, const void *s, size_t n) {
    uint8_t *a = d;
    const uint8_t *b = s;
    while (n--) *a++ = *b++;
    return d;
}

extern int main(void);

void _start(void) {
    main();
    for (;;) __asm__ volatile ("cli; hlt");
}