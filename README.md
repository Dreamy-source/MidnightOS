███╗░░░███╗██╗██████╗░███╗░░██╗██╗░██████╗░██╗░░██╗████████╗
████╗░████║██║██╔══██╗████╗░██║██║██╔════╝░██║░░██║╚══██╔══╝
██╔████╔██║██║██║░░██║██╔██╗██║██║██║░░██╗░███████║░░░██║░░░
██║╚██╔╝██║██║██║░░██║██║╚████║██║██║░░╚██╗██╔══██║░░░██║░░░
██║░╚═╝░██║██║██████╔╝██║░╚███║██║╚██████╔╝██║░░██║░░░██║░░░
╚═╝░░░░░╚═╝╚═╝╚═════╝░╚═╝░░╚══╝╚═╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░

# MidnightOS

<img width="316" height="226" alt="image" src="https://github.com/user-attachments/assets/d50cee46-630f-496e-998c-c98237663944" />
<img width="531" height="308" alt="image" src="https://github.com/user-attachments/assets/1427aef9-62c0-4c59-8993-96ed3958a8f7" />


**32-bit OS written in C and Assembly.**
**Boots with Limine. Own console. Own commands. Own world.**

## NEW UPDATE! [08.04.2026]

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

## Change Log
- Added:
-   Localization Parameter [base, en, ru]
-   Localization Parameter Block [LPB]
-   Get-Lang system [Not finished]
-   Rights Parameter Block [RPB]
-   Get-Rights system [Not finished]

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
