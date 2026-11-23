#include <core.h>
#include <stdint.h>
#include <stddef.h>
#include "font.h"

struct limine_framebuffer 
{
    void *address;
    uint64_t width, height;
    uint64_t pitch;
    uint16_t bpp;
};

struct limine_framebuffer_response 
{
    uint64_t revision;
    uint64_t framebuffer_count;
    struct limine_framebuffer **framebuffers;
};

struct limine_framebuffer_request 
{
    uint64_t id[4];
    uint64_t revision;
    struct limine_framebuffer_response * response;
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request fb_request = {
    .id = { 0xc7b1dd30df4c8b88, 
            0x0a82e883a194f07b,
            0x9d5827dcd881dd75,
            0xa3148604f6fab11b },
    .revision = 0,

};

#define SCALE 2

static struct limine_framebuffer *fb;
static uint64_t cx, cy;
static uint32_t fg = 0xFFFFFF;
static uint32_t bg = 0x101018;

static void put_pixel(uint64_t x, uint64_t  y, uint32_t color)
{
    uint32_t *row = (uint32_t *)((uint8_t *) fb->address + y * fb->pitch);
    row[x] = color;
}

void clear(void)
{
    if (!fb) return;
    for (uint64_t y = 0; y < fb->height; y++)
        for (uint64_t x = 0; x < fb->width; x++)
            put_pixel(x, y, bg);
    cx = cy = 0;
}

void screen_init(void)
{
    if (!fb_request.response || fb_request.response->framebuffer_count < 1)
        return;
    fb = fb_request.response->framebuffers[0];
    clear();
}

static void draw_char(char c, uint64_t col, uint64_t row)
{
    const uint8_t *g = (const uint8_t *)font8x8_basic[(uint8_t)c & 0x7F];

    for ( int gy = 0; gy < 8; gy++)
        for (int gx = 0; gx < 8; gx++)
        {
            uint32_t color = ((g[gy] >> gx) & 1) ? fg : bg;

            for (int sy = 0; sy < SCALE; sy++)
                for (int sx = 0; sx < SCALE; sx++)
                    put_pixel( col * 8 * SCALE + gx * SCALE + sx,
                               row * 8 * SCALE + gy * SCALE +sy, color);
        }   
}

/* Sube todo el texto una linea y deja la ultima linea vacia. */
static void scroll(void)
{
    uint64_t line_h = 8 * SCALE;                  /* alto de una linea en pixeles */
    uint64_t rows   = fb->height / line_h;
    uint64_t line   = line_h * fb->pitch;         /* bytes de una linea de texto */

    /* Copiamos de 8 en 8 bytes: `line` siempre es multiplo de 8 porque
       pitch es multiplo de 4 y line_h es par. */
    uint64_t *dst  = (uint64_t *)fb->address;
    uint64_t *src  = (uint64_t *)((uint8_t *)fb->address + line);
    uint64_t words = (rows - 1) * line / 8;
    for (uint64_t i = 0; i < words; i++)
        dst[i] = src[i];

    for (uint64_t y = (rows - 1) * line_h; y < rows * line_h; y++)
        for (uint64_t x = 0; x < fb->width; x++)
            put_pixel(x, y, bg);
}

void core_putchar(char c)
{
    if (!fb) return;
    uint64_t cols = fb->width / (8 * SCALE);
    uint64_t rows = fb->height / (8 * SCALE);

    if (c == '\n') { cx = 0; cy++; }
    else if (c == '\b') {
        if (cx > 0) {cx--; draw_char(' ', cx, cy); }
    }
    else
    {
        draw_char( c, cx, cy);
        if (++cx >= cols) { cx = 0; cy++; }
    }
    if (cy >= rows) {
        scroll();
        cy = rows - 1;
    }
}

void core_set_colors(uint32_t text, uint32_t back)
{
    fg = text;
    bg = back;
}

CoreScreen Screen =
{
    .init = screen_init,
    .clear = clear,
    .color = core_set_colors,
    .print = printf,
};