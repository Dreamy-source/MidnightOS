███╗░░░███╗██╗██████╗░███╗░░██╗██╗░██████╗░██╗░░██╗████████╗
████╗░████║██║██╔══██╗████╗░██║██║██╔════╝░██║░░██║╚══██╔══╝
██╔████╔██║██║██║░░██║██╔██╗██║██║██║░░██╗░███████║░░░██║░░░
██║╚██╔╝██║██║██║░░██║██║╚████║██║██║░░╚██╗██╔══██║░░░██║░░░
██║░╚═╝░██║██║██████╔╝██║░╚███║██║╚██████╔╝██║░░██║░░░██║░░░
╚═╝░░░░░╚═╝╚═╝╚═════╝░╚═╝░░╚══╝╚═╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░

# MidnightOS

<img width="527" height="306" alt="image" src="https://github.com/user-attachments/assets/c68df996-6d6a-4dc2-a1e5-3c1f3ae03c85" />

**32-bit OS written in C and Assembly.**
**Boots with Limine. Own console. Own commands. Own world.**

## NEW UPDATE! [15.04.2026]

## Features
- Can change kernel in your system! [Data-Rewrite]
- Can change your system fully! [You can re-write kernel or something in your system]
- Guarranted OS-Update every 1-14 Days
- Last update: Full OS-Upgrade
- Custom bootloader (Limine, UEFI)
- IDT, PIC, PIT — interrupts work
- Keyboard input (scancodes → ASCII)
- Console with commands: `hlp`, `clr`, `inf`, `poff`, `rboot`
- Pure C + NASM. No stdlib. No garbage.

## Discord: https://discord.com/channels/1491405695042457620
**Join in Discord if you want to:**
- Discuss MidnightOS
- Share your kernel mods
- Report bugs (but don't cry if you break it yourself in Data-Rewrite mode)

## Build & Run
```bash
echo "Starting Midnight..."
echo "MidnightOS"
qemu-system-x86_64 -kernel kernel.elf -m 256M
echo "Goodbye! ^^"
