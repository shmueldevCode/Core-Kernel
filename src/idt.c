#include <stdint.h>

int printf(const char *fmt, ...);
void clear(void);
void core_set_colors(uint32_t text, uint32_t back);

struct interrupt_frame
{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
};

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtr;

static void idt_set(int vec, void *handler)
{
    uint64_t addr = (uint64_t)handler;
    idt[vec].offset_low  = addr & 0xFFFF;
    idt[vec].selector    = 0x08;
    idt[vec].ist         = 0;
    idt[vec].type_attr   = 0x8E;
    idt[vec].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[vec].offset_high = addr >> 32;
    idt[vec].zero        = 0;
}

__attribute__((noreturn))
static void panic_screen(const char *name, struct interrupt_frame *f,
                         uint64_t err, int has_err) {
                            core_set_colors(0xFFFFFF, 0x8B0000);
                            clear();
                            printf("=== CORE KERNEL PANIC ===\n\n");
                            printf("Excepcion: %s\n", name);
                            printf("RIP: %p\n", (void *)f->ip);
                            if (has_err) printf("Codigo de error: %p\n", (void *)err);
                            printf("\nEl sistema se detuvo.\n");
                            for (;;) __asm__ volatile ("cli; hlt");
                         }


#define EXC(num, name) \
    __attribute__((interrupt)) \
    static void exc_##num(struct interrupt_frame *f) \
    { panic_screen(name, f, 0, 0); }

#define EXC_ERR(num, name) \
    __attribute__((interrupt)) \
    static void exc_##num(struct interrupt_frame *f, uint64_t err) \
    { panic_screen(name, f, err, 1); }

EXC(0,       "Division entre cero")
EXC(1,       "Debug")
EXC(3,       "Breakpoint")
EXC(6,       "Instruccion invalida")
EXC_ERR(8,   "Doble fallo")
EXC_ERR(13,  "Proteccion general")
EXC_ERR(14,  "Fallo de pagina")
EXC(unknown, "Excepcion desconocida")

void idt_init(void)
{
    for (int i = 0; i < 32; i++)
        idt_set(i, exc_unknown);

    idt_set(0,  exc_0);
    idt_set(1,  exc_1);
    idt_set(3,  exc_3);
    idt_set(6,  exc_6);
    idt_set(8,  exc_8);
    idt_set(13, exc_13);
    idt_set(14, exc_14);

    idtr.limit = sizeof(idt) -1;
    idtr.base = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}