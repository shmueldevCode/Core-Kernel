#include <core.h>

int main()
{
    Kernel.init();
    Screen.init();
    Keyboard.init();

    Screen.print("Welcome to Malcore\n");
    Screen.print("RAM usable: %d MB\n", (int)(Memory.total() / 1024 / 1024));
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