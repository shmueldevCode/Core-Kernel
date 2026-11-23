#include <stdint.h>
#include "io.h"
#include "internal.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0X21
#define PIC2_CMD  0XA0
#define PIC2_DATA  0XA1

void core_pic_init(void)
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

void core_pic_unmask(uint8_t irq)
{
    if (irq >= 8) {
        /* Las IRQ 8-15 viven en el PIC esclavo, que entra al maestro por la
           IRQ2 (la "cascada"). Si la IRQ2 sigue enmascarada, nunca llegan. */
        outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << 2));
        outb(PIC2_DATA, inb(PIC2_DATA) & ~(1 << (irq - 8)));
    } else {
        outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << irq));
    }
}

void core_pic_eoi(uint8_t irq)
{
    if (irq >=8) outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}