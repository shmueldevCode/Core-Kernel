#include <stdint.h>
#include "io.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0X21
#define PIC2_CMD  0XA0
#define PIC2_DATA  0XA1

void pic_init(void)
{
    outb(PIC1_CMD, 0x11);  io_wait();
    outb(PIC2_CMD, 0x11);  io_wait();

    outb(PIC1_DATA, 0x20); io_wait();
    outb(PIC2_DATA, 0x28); io_wait();

    outb(PIC1_DATA, 0x04); io_wait();
    outb(PIC2_DATA, 0x02); io_wait();

    outb(PIC1_DATA, 0x01); io_wait();
    outb(PIC2_DATA, 0x01); io_wait();

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_unmask(uint8_t irq)
{
    uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;
    if (irq >=8) irq -=8;
    outb(port, inb(port) & ~(1 << irq));
}

void pic_eoi(uint8_t irq)
{
    if (irq >=8) outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}