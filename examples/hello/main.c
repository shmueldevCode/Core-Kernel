#include <core.h>

int main() 
{
    Kernel.init();
    Screen.init();
    Keyboard.init();

    while (1)
    {
        char c = Keyboard.read();
        Screen.print("%c", c);
        if (c == '\n') Screen.print("> ");
    }
}