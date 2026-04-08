████╗░████║██║██╔══██╗████╗░██║██║██╔════╝░██║░░██║╚══██╔══╝
██╔████╔██║██║██║░░██║██╔██╗██║██║██║░░██╗░███████║░░░██║░░░
██║╚██╔╝██║██║██║░░██║██║╚████║██║██║░░╚██╗██╔══██║░░░██║░░░
██║░╚═╝░██║██║██████╔╝██║░╚███║██║╚██████╔╝██║░░██║░░░██║░░░
╚═╝░░░░░╚═╝╚═╝╚═════╝░╚═╝░░╚══╝╚═╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░

# MidnightOS

**32-bit OS written in C and Assembly.**
**Boots with Limine. Own console. Own commands. Own world.**

## Features
- Guarranted OS-Update every 1-7 Days
- Custom bootloader (Limine, UEFI+BIOS)
- IDT, PIC, PIT — interrupts work
- Keyboard input (scancodes → ASCII)
- Console with commands: `hlp`, `clr`, `inf`, `poff`, `rboot`
- Pure C + NASM. No stdlib. No garbage.

## Build & Run
```bash
echo "Starting Midnight..."
echo "MidnightOS"
qemu-system-x86_64 -kernel kernel.elf -m 256M
echo "Goodbye! ^^"

<img width="316" height="226" alt="image" src="https://github.com/user-attachments/assets/d50cee46-630f-496e-998c-c98237663944" />
<img width="309" height="128" alt="image" src="https://github.com/user-attachments/assets/cd52655e-dd4b-4d0a-af1f-d8661745be20" />
