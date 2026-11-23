#ifndef CORE_INTERNAL_H
#define CORE_INTERNAL_H

#include <stdint.h>

/*
 * Funciones internas de Core, compartidas entre los archivos de src/.
 *
 * Todas llevan el prefijo core_ para no chocar con las funciones del usuario:
 * si tu kernel define su propio sleep(), clear() o timer_init(), tiene que
 * enlazar sin problemas. Todo lo que no se comparte entre archivos es static.
 *
 * El usuario no necesita este archivo: su API esta en include/core.h.
 */

void core_memory_init(void);
void core_gdt_init(void);
void core_idt_init(void);
void core_idt_set(int vec, void *handler);
void core_pic_init(void);
void core_pic_unmask(uint8_t irq);
void core_pic_eoi(uint8_t irq);
void core_timer_init(void);
void core_putchar(char c);

#endif
