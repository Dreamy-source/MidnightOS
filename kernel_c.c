void outw(unsigned short port, unsigned short val);
void outb(unsigned short port, unsigned char val);
void putchar(char c, int color);
void print(const char* str, int color);
void clear_screen(void);
void console_prefix(void);
void scroll_screen(void);
void handle_enter(void);
void disable_bios_cursor(void);
void execute_command(const char* cmd);
int strcmp(const char* a, const char* b);
void console_putchar(char c);
void console_backspace(void);
void console(void);

char* video = (char*)0xB8000;
int cursor_x = 0;
int cursor_y = 0;
int min_x = 0;
int min_y = 0;
int max_x = 60;

static char input_buffer[256];
static int input_len = 0;

void scroll_screen(void) {
    for (int i = 0; i < 24 * 80 * 2; i++) {
        video[i] = video[i + 160];
    }
    for (int i = 24 * 80 * 2; i < 25 * 80 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x0F;
    }
}

void putchar(char c, int color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        int pos = (cursor_y * 80 + cursor_x) * 2;
        video[pos] = c;
        video[pos + 1] = color;
        cursor_x++;
    }
    
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= 25) {
        scroll_screen();
        cursor_y = 24;
    }
}

void print(const char* str, int color) {
    for (int i = 0; str[i]; i++) {
        putchar(str[i], color);
    }
}

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x0F;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void outb(unsigned short port, unsigned char val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void outw(unsigned short port, unsigned short val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

void disable_bios_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void console_prefix() {
    print("[midnight@user /] > ", 0x0F);
}

int strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a - *b;
}

void cmd_poff(void) {
    print("Shutting down...\n", 0x0F);
    
    outw(0x604, 0x2000);
    
    outb(0x64, 0xFE);
    
    asm volatile("cli");
    asm volatile("hlt");
    
    unsigned int* p = (unsigned int*)0xFFFFFFFF;
    *p = 0;
}

void cmd_rboot(void) {
    print("Rebooting...\n", 0x0F);
    
    outb(0x64, 0xFE);
    
    asm volatile("cli");
    asm volatile("hlt");
    unsigned int* p = (unsigned int*)0xFFFFFFFF;
    *p = 0;
}

void execute_command(const char* cmd) {
    if (strcmp(cmd, "hlp") == 0) {
        print("hlp - Command list\n", 0x0F);
        print("clr - Clear screen\n", 0x0F);
        print("inf - Information\n", 0x0F);
        print("poff - Powering off computer\n", 0x0F);
        print("rboot - Rebooting computer\n", 0x0F);
    }
    else if (strcmp(cmd, "clr") == 0) {
        clear_screen();
        console_prefix();
        min_x = cursor_x;
        min_y = cursor_y;
    }
    else if (strcmp(cmd, "inf") == 0) {
        print("MidnightOS v0.1\n", 0x0F);
        print("Build: 08.04.2026\n", 0x0F);
    }
    else if (strcmp(cmd, "poff") == 0) {
        cmd_poff();
    }
    else if (strcmp(cmd, "rboot") == 0) {
        cmd_rboot();
    }
    else if (cmd[0] != '\0') {
        print("Command not found: ", 0x0C);
        print(cmd, 0x0C);
        print("\n", 0x0C);
    }
}

void console_putchar(char c) {
    if (input_len < 255) {
        input_buffer[input_len++] = c;
        putchar(c, 0x0F);
    }
}

void console_backspace(void) {
    if (input_len > 0) {
        input_len--;
        if (cursor_x > min_x) {
            cursor_x--;
            putchar(' ', 0x0F);
            cursor_x--;
        }
    }
}

void handle_enter(void) {
    input_buffer[input_len] = '\0';

    cursor_x = 0;
    cursor_y++;
    
    if(cursor_y >= 25) {
        scroll_screen();
        cursor_y = 24;
    }
    
    execute_command(input_buffer);
    console_prefix();
    input_len = 0;
    
    min_x = cursor_x;
    min_y = cursor_y;
}

/*
// ===============================
// LOCALIZATION PARAMETER BLOCK
// ===============================
void user_localization(void) {
  localize = {
    en = "en";
    ru = "ru";
    // others
  }
  base = localize.en;

  en = localize.en;
  ru = localize.ru;

  chs = {
    en = true;
    ru = false;
  }
}
*/

/*
void DEV_DEBUG() {
  CONSOLE = {
    COMMANDS = {
      HLP = {
        DEV_DEBUG_CLR_RBOOT = "hlp"
        DEV_DEBUG_CLR_DESC = {[[
          Command list.
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 1;
      }
      CLR = {
        DEV_DEBUG_CLR_RBOOT = "clr"
        DEV_DEBUG_CLR_DESC = {[[
          Clearing screen [CONSOLE].
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 2;
      }
      INF = {
        DEV_DEBUG_INF = "inf"
        DEV_DEBUG_INF_DESC = {[[
          Print information:
          MidnightOS v0.0
          Build: 00.00.0000
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 3;
      }
      POFF = {
        DEV_DEBUG_POFF = "poff"
        DEV_DEBUG_POFF_DESC = {[[
          Powering off computer.
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 4;
      }
      RBOOT = {
        DEV_DEBUG_RBOOT = "rboot"
        DEV_DEBUG_RBOOT_DESC = {[[
          Rebooting computer.
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 5;
      }
    }
  }
}
*/

extern void setup_idt();
extern void setup_pic();
extern void setup_pit();

void kernel_c() {
    clear_screen();
    disable_bios_cursor();
    
    print("====================\n", 0x0D);
    print("     MidnightOS\n", 0x0D);
    print("====================\n", 0x0D);
    print("Booting from ", 0x0F);
    print("limine\n", 0x0B);
    print("Starting...\n", 0x0F);
    print("Getting: C\n", 0x0F);
    print("Initializing system...\n", 0x0F);
    print("Setting up: IDT, PIC, PIT\n", 0x0F);
    print("[ ON ] IDT\n", 0x0A);
    print("[ ON ] PIC\n", 0x0A);
    print("[ ON ] PIT\n", 0x0A);
    print("[ ON ] Kernel\n", 0x0A);
    print("Midnight initialized!\n", 0x0A);
    
    setup_idt();
    setup_pic();
    setup_pit();
    asm volatile("sti");
    
    cursor_x = 0;
    cursor_y = 14;
    console_prefix();
    
    min_x = cursor_x;
    min_y = cursor_y;
    
    while(1) {
        asm volatile("hlt");
    }
}
