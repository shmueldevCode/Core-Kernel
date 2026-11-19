# Core-Kernel# Core

**Crea tu propio kernel en 5 líneas de C.**

Siempre me pareció injusto que hacer un sistema operativo fuera algo reservado para gente que ya sabe un montón de ensamblador, linkers, manuales de Intel blablabla. Cuando empecé con mi propio OS (MalCore) me di cuenta de que el 90% del tiempo se va en lo mismo: arrancar, pintar la pantalla, configurar tablas raras del CPU... y recién después puedes hacer lo divertido.

Core es mi intento de arreglar eso. Es un framework en C que se encarga de todo lo pesado para que tú solo escribas esto:

```c
#include <core.h>

int main()
{
    Create.Kernel();
    Create.Screen();

    printf("Welcome to MalCore\n");

    return 0;
}
```

Compilas, arranca en QEMU, y eso que ves en pantalla lo está dibujando **tu kernel**. Sin sistema operativo debajo. Nada de Linux, nada de Windows. Solo tu código y el CPU.

> **Core está en desarrollo temprano.** Funciona, pero todavía le falta mucho. Lo voy construyendo poco a poco y documentando el camino.

---

## ¿Qué hace por ahora?

**v0.1 — Primer arranque**
- Arranca con [Limine](https://github.com/limine-bootloader/limine) (BIOS y UEFI)
- `Create.Screen()` pide una pantalla gráfica y la limpia
- `printf` propio que dibuja texto letra por letra, píxel por píxel (soporta `%d`, `%u`, `%x`, `%s`, `%c`, `%p`)

**v0.2 — El kernel se defiende**
- `Create.Kernel()` carga una GDT y una IDT propias
- Si algo explota (división entre cero, page fault, instrucción inválida...), en vez de reiniciarse o congelarse, Core te muestra una pantalla roja de **KERNEL PANIC** con el error y la dirección exacta donde pasó

Que parece poco, pero ahí adentro ya hay manejo de interrupciones del CPU funcionando.

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

Se abre QEMU con tu kernel. Si quieres ver la pantalla de pánico en acción:

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
│   ├── core.c          ← el verdadero inicio (_start) y el objeto Create
│   ├── gdt.c           ← segmentos de memoria
│   ├── idt.c           ← tabla de interrupciones + pantalla de pánico
│   ├── screen.c        ← framebuffer y dibujo de letras
│   ├── printf.c        ← printf hecho desde cero
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

Este es el plan. Cada versión depende de la anterior, así que voy en orden:

- [x] **v0.1** — Arranque, pantalla y `printf`
- [x] **v0.2** — GDT, IDT y manejo de excepciones
- [ ] **v0.3** — Timer y teclado (`sleep()`, `Create.Keyboard()`)
- [ ] **v0.4** — Memoria: paginación y heap (`kmalloc` / `kfree`)
- [ ] **v0.5** — Procesos y multitarea (`Create.Task()`)
- [ ] **v0.6** — Seguridad: modo usuario y syscalls
- [ ] **v0.7** — Sistema de archivos
- [ ] **v0.8** — Drivers propios con una interfaz simple
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

Hecho con mucho café y muchos `triple fault` por **[shmueldevCode](https://github.com/shmueldevCode)**