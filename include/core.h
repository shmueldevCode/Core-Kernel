#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define KEY_UP     0x11
#define KEY_DOWN   0x12
#define KEY_LEFT   0x13
#define KEY_RIGHT  0x14
#define CTRL(c)    ((c) & 0x1F)

typedef struct {
    void (*init)(void);
} CoreKernel;

typedef struct {
    void (*init)(void);
    void (*clear)(void);
    void (*color)(uint32_t text, uint32_t back);
    int  (*print)(const char *fmt, ...);
} CoreScreen;

typedef struct {
    void     (*sleep)(uint64_t ms);
    uint64_t (*uptime)(void);
} CoreTimer;

typedef struct {
    void (*init)(void);
    char (*read)(void);
    int (*available)(void);
} CoreKeyboard;

typedef struct {
    uint64_t (*total)(void);
    uint64_t (*available)(void);
    void *(*page_alloc)(void);
    void  (*page_free)(void *page);
    void    *(*alloc)(uint64_t size);
    void    (*free)(void *ptr);
} CoreMemory;

extern CoreKernel Kernel;
extern CoreScreen Screen;
extern CoreTimer  Timer;
extern CoreKeyboard Keyboard;
extern CoreMemory Memory;

int printf(const char *fmt, ...);

#endif