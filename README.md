# Core

**Crea tu propio kernel en pocas líneas de C.**

Siempre me pareció injusto que hacer un sistema operativo fuera algo reservado para gente que ya sabe un montón de ensamblador, linkers y manuales de Intel blablabla. Cuando empecé con mi propio OS (MalCore) me di cuenta de que el 90% del tiempo se va en lo mismo: arrancar, pintar la pantalla, configurar tablas raras del CPU... y recién después puedes hacer lo divertido.

Core es mi intento de arreglar eso. Es un framework en C que se encarga de todo lo pesado para que tú solo escribas esto:

```c
#include <core.h>

int main()
{
    Kernel.init();
    Screen.init();
    Keyboard.init();

    Screen.print("Welcome to MalCore\n");
    Screen.print("RAM: %d MB\n", (int)(Memory.total() / 1024 / 1024));

    while (1) {
        char c = Keyboard.read();
        Screen.print("%c", c);
    }
}
```

Compilas, arranca en QEMU, y eso que ves en pantalla lo está dibujando **tu kernel**. Sin sistema operativo debajo. Nada de Linux, nada de Windows. Solo tu código y el CPU.

> 🚧 **Core está en desarrollo.** Funciona, pero todavía le falta mucho. Lo voy construyendo poco a poco y documentando el camino.

---

## La idea: todo por módulos

Cada parte del sistema es un módulo con sus propias funciones. Así no tienes que memorizar mil funciones sueltas, todo está ordenado:

| Módulo | Qué hace | Ejemplo |
|---|---|---|
| `Kernel` | Prepara el CPU, la memoria y las interrupciones | `Kernel.init()` |
| `Screen` | Pantalla gráfica y texto | `Screen.print("hola %d", 5)`, `Screen.clear()` |
| `Timer` | Tiempo real en milisegundos | `Timer.sleep(1000)`, `Timer.uptime()` |
| `Keyboard` | Leer teclas | `Keyboard.read()`, `Keyboard.available()` |
| `Memory` | RAM dinámica | `Memory.alloc(64)`, `Memory.free(ptr)` |

---

## ¿Qué hace por ahora?

**v0.1 — Primer arranque**
- Arranca con [Limine](https://github.com/limine-bootloader/limine)
- Pide una pantalla gráfica y dibuja texto letra por letra, píxel por píxel
- `printf` hecho desde cero (`%d`, `%u`, `%x`, `%s`, `%c`, `%p`)

**v0.2 — El kernel se defiende**
- GDT e IDT propias
- Si algo explota (división entre cero, page fault, instrucción inválida...), en vez de reiniciarse o congelarse, te muestra una pantalla roja de **CORE KERNEL PANIC** con el error y la dirección exacta donde pasó

**v0.3 — Tiempo y teclado**
- El PIC reprogramado para que el hardware no choque con las excepciones del CPU
- Timer a 1000 Hz: `Timer.sleep(ms)` duerme el CPU de verdad (con `hlt`), no gasta 100% dando vueltas
- Teclado con Shift, Bloq Mayús, Ctrl (`CTRL('c')`, `CTRL('l')`...) y flechas
- La API pasó a módulos (`Kernel.init()`, `Screen.print()`...)

**v0.4 — Memoria**
- Lee el mapa de memoria que da Limine y sabe cuánta RAM hay de verdad
- Allocator de páginas de 4 KB (`Memory.page_alloc()` / `Memory.page_free()`)
- Heap con `Memory.alloc()` y `Memory.free()`, que divide y junta bloques para no desperdiciar memoria

---

## Cómo probarlo

Yo lo desarrollo en Arch Linux. Necesitas esto:

```bash
sudo pacman -S clang lld make qemu-full xorriso limine
```

Clonas y corres:

```bash
git clone https://github.com/shmueldevCode/Core-Kernel.git
cd Core-Kernel
make run
```

Se abre QEMU con tu kernel. Haz clic dentro de la ventana y escribe.

Si quieres ver la pantalla de pánico en acción:

```bash
make clean && make run EXAMPLE=panic
```

Ese ejemplo hace una división entre cero a propósito. 💥

---

## Cómo está organizado

```
Core-Kernel/
├── include/
│   └── core.h          ← lo único que necesita ver el usuario
├── src/
│   ├── core.c          ← el verdadero inicio (_start) y Kernel.init()
│   ├── gdt.c           ← segmentos de memoria
│   ├── idt.c           ← tabla de interrupciones + pantalla de pánico
│   ├── pic.c           ← reparte las interrupciones del hardware
│   ├── timer.c         ← reloj de 1000 Hz
│   ├── keyboard.c      ← teclado PS/2
│   ├── memory.c        ← páginas y heap
│   ├── screen.c        ← framebuffer y dibujo de letras
│   ├── printf.c        ← printf hecho desde cero
│   ├── io.h            ← hablar con el hardware por puertos
│   └── font.h          ← fuente 8x8
├── examples/
│   ├── hello/          ← el ejemplo básico
│   └── panic/          ← para ver explotar el kernel
├── linker.ld
├── limine.conf
└── Makefile
```

La idea es que tú escribas tu kernel en `examples/` (o en tu propio proyecto) y nunca tengas que tocar `src/` si no quieres.

---

## Hacia dónde va

Cada versión depende de la anterior, así que voy en orden:

- [x] **v0.1** — Arranque, pantalla y `printf`
- [x] **v0.2** — GDT, IDT y manejo de excepciones
- [x] **v0.3** — Timer y teclado
- [x] **v0.4** — Memoria: páginas y heap
- [ ] **v0.5** — Procesos y multitarea (`Task.create()`)
- [ ] **v0.6** — Seguridad: tablas de páginas propias, modo usuario y syscalls
- [ ] **v0.7** — Sistema de archivos
- [ ] **v0.8** — Drivers propios con una interfaz simple (y teclado en español, con ñ y tildes)
- [ ] **v1.0** — Tests, estabilidad y documentación completa
- [ ] **Extra** — `core build` en Python para que sea todavía más fácil

La meta es llegar a la v1.0 antes de terminar la carrera y presentarlo como mi trabajo final.

---

## ¿Y MalCore?

MalCore es mi sistema operativo. Core es la herramienta con la que lo estoy construyendo. La idea es que al final MalCore sea la prueba de que Core funciona de verdad.

---

## ¿Quieres ayudar?

Si encuentras un bug, tienes una idea o simplemente quieres preguntar algo, abre un issue. Estoy aprendiendo mientras lo hago, así que cualquier consejo se agradece un montón.

---

## Licencia

MIT. Úsalo, rómpelo, mejóralo. Solo deja el crédito.

Hecho con mucho café y muchos `triple fault` por **[shmueldevCode](https://github.com/shmueldevCode)** desde Perú 🇵🇪