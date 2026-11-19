#include <stdint.h>
#include <core.h>
#include "io.h"

void pic_unmask(uint8_t irq);
void pic_eoi(uint8_t irq);
void idt_set(int vec, void *handler);

struct interrupt_frame;

static const char keymap[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ',
};

static const char keymap_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','"','~',
    0, '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' ',
};

#define BUF_SIZE 128
static volatile char buffer[BUF_SIZE];
static volatile uint8_t head = 0, tail = 0;
static volatile int shift = 0;

__attribute__((interrupt))
static void keyboard_handler(struct interrupt_frame *f)
{
    (void)f;
    uint8_t sc = inb(0x60);

    if (sc == 0x2A || sc == 0x36) shift = 1;
    else if (sc == 0xAA || sc == 0XB6) shift = 0;
    else if (!(sc & 0x80))
    {
        char c = shift ? keymap_shift[sc] : keymap[sc];
        uint8_t next = (head + 1) % BUF_SIZE;
        if (c && next != tail)
        {
            buffer[head] = c;
            head = next;
        }
    }
    pic_eoi(1);
}

static void keyboard_init(void)
{
    while (inb(0x64) & 1) inb(0x60);
    idt_set(33, keyboard_handler);
    pic_unmask(1);
}

static int Keyboard_available(void)
{
    return head != tail;
}

static char keyboard_read(void)
{
    while (head == tail)
        __asm__ volatile ("hlt");
    char c = buffer[tail];
    tail = (tail + 1) % BUF_SIZE;
    return c;
}

CoreKeyboard Keyboard = {
    .init = keyboard_init,
    .read = keyboard_read,
    .available = Keyboard_available,
};
