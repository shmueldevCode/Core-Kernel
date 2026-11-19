#ifndef CORE_H
#define CORE_H

#include <stdint.h>

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

extern CoreKernel Kernel;
extern CoreScreen Screen;
extern CoreTimer  Timer;
extern CoreKeyboard Keyboard;

int printf(const char *fmt, ...);

#endif