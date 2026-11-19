#ifndef KORE_H
#define KORE_H

typedef  struct
{
    void (*Kernel)(void);
    void (*Screen)(void);
} KoreCreate;

extern KoreCreate Create;

int printf(const char *fmt, ...);
void clear(void);

#endif
