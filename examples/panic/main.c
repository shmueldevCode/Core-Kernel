#include <core.h>

int main()
{
    Kernel.init();
    Screen.init();

    Screen.print("Welcome to Malcore\n");
    Screen.print("Este ejemplo va a dividir entre cero a proposito.\n\n");

    for (int i = 3; i > 0; i--) {
        Screen.print("%d... ", i);
        Timer.sleep(1000);
    }

    /* volatile: obliga al compilador a hacer la division de verdad en vez de
       detectarla y eliminarla al compilar. */
    volatile int zero = 0;
    int boom = 10 / zero;

    /* Nunca se llega aqui: el CPU lanza la excepcion 0 y Core muestra
       la pantalla de CORE KERNEL PANIC. */
    Screen.print("%d\n", boom);
    return 0;
}
