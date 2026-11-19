#include <kore.h>

int main() 
{
    Create.Kernel();
    Create.Screen();

    printf("Welcome to Malcore\n");
    printf("Made in Kore v%d.%d\n", 0, 1);

    volatile int cero = 0;
    printf("%d\n", 10 / cero);

    return 0;
}