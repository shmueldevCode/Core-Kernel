#include <stdint.h>
#include <core.h>
#include "io.h"

void pic_unmask(uint8_t irq);
void pic_eoi(uint8_t irq);
void idt_set(int vec, void *handler);

struct interrupt_frame;

static volatile uint64_t ticks = 0;

__attribute__((interrupt))
static void timer_handler(struct interrupt_frame *f)
{
    (void)f;
    ticks++;
    pic_eoi(0);
}

void timer_init(void)
{
    uint16_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);

    idt_set(32, timer_handler);
    pic_unmask(0);
}

uint64_t uptime_ms(void) { return ticks; }

void sleep(uint64_t ms)
{
    uint64_t end = ticks + ms;
    while (ticks < end)
        __asm__ volatile ("hlt");
}

CoreTimer Timer = {
    .sleep  = sleep,
    .uptime = uptime_ms,
};