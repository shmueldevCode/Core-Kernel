#include <stdint.h>
#include <core.h>
#include "io.h"
#include "internal.h"

struct interrupt_frame;

static volatile uint64_t ticks = 0;

__attribute__((interrupt))
static void timer_handler(struct interrupt_frame *f)
{
    (void)f;
    ticks++;
    core_pic_eoi(0);
}

void core_timer_init(void)
{
    uint16_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);

    core_idt_set(32, timer_handler);
    core_pic_unmask(0);
}

static uint64_t timer_uptime(void) { return ticks; }

static void timer_sleep(uint64_t ms)
{
    uint64_t end = ticks + ms;
    while (ticks < end)
        __asm__ volatile ("hlt");
}

CoreTimer Timer = {
    .sleep  = timer_sleep,
    .uptime = timer_uptime,
};
