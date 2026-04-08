███╗░░░███╗██╗██████╗░███╗░░██╗██╗░██████╗░██╗░░██╗████████╗
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
qemu-system-x86_64 -kernel kernel.elf -m 256M
echo "MidnightOS"
