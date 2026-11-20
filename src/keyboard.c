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
static volatile int shift = 0, caps = 0, ctrl = 0, alt = 0, extended = 0;

static void push(char c)
{
    uint8_t next = (head + 1) % BUF_SIZE;
    if (next != tail) {
        buffer[head] = c;
        head = next;
    }
}

static int is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

__attribute__((interrupt))
static void keyboard_handler(struct interrupt_frame *f)
{
    (void)f;
    uint8_t sc = inb(0x60);

    if (sc == 0xE0) {
        extended = 1;
        pic_eoi(1);
        return;
    }

    int released = sc & 0x80;
    uint8_t code = sc & 0x7F;

    if (extended) {
        extended = 0;
        if (code == 0x1D) ctrl = !released;
        else if (code == 0x38) alt = !released;
        else if (!released) {
            if (code == 0x48) push(KEY_UP);
            if (code == 0x50) push(KEY_DOWN);
            if (code == 0x4B) push(KEY_LEFT);
            if (code == 0x4D) push(KEY_RIGHT);
        }
        pic_eoi(1);
        return;
    }

    switch (code) {
        case 0x2A: case 0x36: shift = !released; break;
        case 0x1D:            ctrl  = !released; break;
        case 0x38:            alt   = !released; break;
        case 0x3A:            if (!released) caps = !caps; break;
        default:
            if (!released) {
                char c = shift ? keymap_shift[code] : keymap[code];
                if (caps && is_letter(c)) c ^= 0x20;
                if (ctrl && is_letter(c)) c = CTRL(c);
                if (c) push(c);
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

static int keyboard_available(void)
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
    .init      = keyboard_init,
    .read      = keyboard_read,
    .available = keyboard_available,
};