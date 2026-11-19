#ifndef CORE_H
#define CORE_H

typedef  struct
{
    void (*Kernel)(void);
    void (*Screen)(void);
} CoreCreate;

extern CoreCreate Create;

int printf(const char *fmt, ...);
void clear(void);

#endif
