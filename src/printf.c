#include <stdarg.h>
#include <stdint.h>

void kore_putchar(char c);

static int print_str(const char *s)
{
    int n = 0;
    while (*s)
    {
        kore_putchar(*s++); n++; 
    }
    return n;
}

static int print_num(uint64_t v, unsigned base, int neg)
{
    char buf[32];
    int i = 0, n = 0;

    if (neg) {kore_putchar('-'); n++; }
    do
    {
        buf[i++] = "0123456789abcdef"[v % base];
        v /= base;
    }while (v);
    while (i) { kore_putchar(buf[--i]); n++; }
    return n;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = 0;

    for (; *fmt; fmt++)
    {
        if (*fmt != '%') { kore_putchar(*fmt); n++; continue;}

        switch (*++fmt)
        {
            case 'd': 
            {
                int x = va_arg(ap, int);
                n += x < 0 ? print_num((uint64_t)(-(int64_t)x), 10, 1)
                           : print_num((uint64_t)x, 10, 0);
                break;
            }
            case 'u': n += print_num(va_arg(ap, unsigned), 10, 0); break;
            case 'x': n += print_num(va_arg(ap, unsigned), 16, 0); break;
            case 's':
            {
                const char *s = va_arg(ap, const char *);
                n += print_str(s ? s : "(null)");
                break;
            }
            case 'c': kore_putchar((char)va_arg(ap, int)); n++; break;
            case '%': kore_putchar('%'); n++; break;
            case '\0': fmt--; break;
            default: kore_putchar('%'); kore_putchar(*fmt); n += 2;
        }
    }

    va_end(ap);
    return n;
}