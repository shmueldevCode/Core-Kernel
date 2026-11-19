#include <core.h>

int main() 
{
    Kernel.init();
    Screen.init();

    printf("Welcome to Malcore\n");
    printf("Made in Core v%d.%d\n", 0, 1);

    for (int i = 3; i > 0; i--)
    {
        Screen.print("%d...\n", i);
        Timer.sleep(1000);
    }

    Screen.print("Timer OK: %d ms desde el arranque\n", (int)Timer.uptime());
    return 0;
}