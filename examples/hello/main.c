#include <core.h>

int main()
{
    Kernel.init();
    Screen.init();
    Keyboard.init();

    Screen.print("Welcome to Malcore\n");
    Screen.print("RAM usable: %d MB\n", (int)(Memory.total() / 1024 / 1024));
    Screen.print("RAM libre: %d MB\n\n", (int)(Memory.available() / 1024 / 1024));

    char *msg = Memory.alloc(32);
    const char *src = "Hola desde el heap!";
    int i = 0;
    while (src[i]) { msg[i] = src[i]; i++; }
    msg[i] = 0;
    Screen.print("%s (en %p)\n", msg, msg);

    int *nums = Memory.alloc(10 * sizeof(int));
    for (int j = 0; j < 10; j++) nums[j] = j * j;
    Screen.print("nums[9] = %d\n", nums[9]);

    Memory.free(msg);
    Memory.free(nums);

    void *again = Memory.alloc(32);
    Screen.print("Reusado en: %p\n", again);
    Memory.free(again);

    Screen.print("\nEscribe algo:\n> ");

    while (1)
    {
        char c = Keyboard.read();

        if (c == CTRL('l'))      Screen.clear();
        else if (c == CTRL('c')) Screen.print("^C\n> ");
        else if (c == KEY_UP)    Screen.print("[arriba]");
        else if (c == KEY_DOWN)  Screen.print("[abajo]");
        else if (c == KEY_LEFT)  Screen.print("[izq]");
        else if (c == KEY_RIGHT) Screen.print("[der]");
        else {
            Screen.print("%c", c);
            if (c == '\n') Screen.print("> ");
        }
    }
}