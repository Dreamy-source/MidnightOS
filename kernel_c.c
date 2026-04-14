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

// ===============================
// LOCALIZATION PARAMETER BLOCK
// ===============================
struct localization_array {
    char* base;
    char* en;
    char* ru;
    
    int base_state;
    int en_state;
    int ru_state;
};

struct localization_array localization = {
  .base = "en",
  .en = "en",
  .ru = "ru",
  
  .base_state = 1,
  .en_state = 0,
  .ru_state = 0
  
  // 1 - ON
  // 0 - OFF
};

void lpb_get_lang(void) {
  if (localization.base_state == 1 && localization.en_state == 0 && localization.ru_state == 0) {
    print("Lang: en [base]\n", 0x07);
  }
  else if (localization.en_state == 1 && localization.base_state == 0 && localization.ru_state == 0) {
    print("Lang: en\n", 0x07);
  }
  else if (localization.ru_state == 1 && localization.base_state == 0 && localization.en_state == 0) {
    print("Lang: ru\n", 0x07);
  }
  else {
    print("No language selected. Using .base_state parameter from file: FILE_FILE_FILE.p\n", 0x07);
  }
}

void base_loc_system_answer(void) {
  print("====================\n", 0x0D);
  print("     MidnightOS\n", 0x0D);
  print("====================\n", 0x0D);
  lpb_get_lang();
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
}

void en_loc_system_answer(void) {
  print("====================\n", 0x0D);
  print("     MidnightOS\n", 0x0D);
  print("====================\n", 0x0D);
  lpb_get_lang();
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
}

/*
void ru_loc_system_answer(void) {
  print("====================\n", 0x0D);
  print("     MidnightOS\n", 0x0D);
  print("====================\n", 0x0D);
  lpb_get_lang();
  print("Загрузчик: ", 0x0F);
  print("limine\n", 0x0B);
  print("Запуск...\n", 0x0F);
  print("Загрузка: C\n", 0x0F);
  print("Инициализирование системы...\n", 0x0F);
  print("Настройка: IDT, PIC, PIT\n", 0x0F);
  print("[ ВКЛ ] IDT\n", 0x0A);
  print("[ ВКЛ ] PIC\n", 0x0A);
  print("[ ВКЛ ] PIT\n", 0x0A);
  print("[ ВКЛ ] Kernel\n", 0x0A);
  print("Midnight инициализирована!\n", 0x0A);
}
*/

// =========================
// RIGHTS PARAMETER BLOCK
// =========================
struct rights_array {
  char* AP;
  char* A;
  char* U;
  
  char* user;
  char* current;
};

struct rights_array rights;

void rights_init(void) {
  rights.AP = "Alpha-Prime";  // for Data-Rewrite
  rights.A = "Alpha";  // for User
  rights.U = "User";  // for User
  
  rights.user = rights.U;   // user
  
  rights.current = rights.user;
  
  if (rights.current == rights.user || rights.current == rights.U || rights.current == rights.A || rights.current == rights.AP) {
    print("Rights: ", 0x0F);
    print(rights.current, 0x0F);
    print("\n", 0x0F);
    print("Access: Read, Write\n", 0x0F);
  };
  
  /*
  AP - System, Kernel, [dreamy]
  A - User max rights
  U - User base rights
  */
};

void execute_command(const char* cmd) {
    if (strcmp(cmd, "hlp") == 0) {
        print("hlp - Command list\n", 0x0F);
        print("clr - Clear screen\n", 0x0F);
        print("inf - Information\n", 0x0F);
        print("rights - Your rights\n", 0x0F);
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
        print("MidnightOS v1.9\n", 0x0F);
        print("Build: 08.04.2026\n", 0x0F);
    }
    else if (strcmp(cmd, "poff") == 0) {
        cmd_poff();
    }
    else if (strcmp(cmd, "rboot") == 0) {
        cmd_rboot();
    }
    else if (strcmp(cmd, "rights") == 0) {
        rights_init();
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
void DEV_DEBUG() {
  CONSOLE = {
    COMMANDS = {
      HLP = {
        DEV_DEBUG_CLR = "hlp"
        DEV_DEBUG_CLR_DESC = {[[
          Command list.
          =====================
        ]]}
        DEV_PARTITION_QUEUE_CLR = 1;
      }
      CLR = {
        DEV_DEBUG_CLR = "clr"
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

// ==============================
// KERNEL PARAMETER BLOCK
// ==============================
struct KERNEL_ARRAY {
    int kernel_after;
};

void KERNEL_FAILURE(void) {
    kernel_array.kernel_after = 0;
    
    print("KERNEL FAILURE.\n", 0x0C);           // красный
    print("Host: kernel.asm in /dreamy/Midnight/AP/Core/Kernel.asm\n", 0x0C);
    print("\n", 0x0F);
    print("██╗░░██╗\n", 0x0F);
    print("╚═╝░██╔╝\n", 0x0F);
    print("░░░██╔╝░\n", 0x0F);
    print("░░░╚██╗░\n", 0x0F);
    print("██╗░╚██╗\n", 0x0F);
    print("╚═╝░░╚═╝\n", 0x0F);
    
    asm volatile("cli");
    asm volatile("hlt");
};

struct KERNEL_ARRAY kernel_array;

void KERNEL_AFTER(void) {
    kernel_array.kernel_after = 1;
    
    if (kernel_array.kernel_after == 1) {
        print("Main complete finished.\n", 0x0F);
        print("Starting kernel...\n", 0x0F);
    }
    else {
        print("CRITICAL: ", 0x0F);
        print("Kernel Error\n", 0x0C);
        KERNEL_FAILURE();
    }
}

extern void setup_idt();
extern void setup_pic();
extern void setup_pit();

void kernel_c() {
    clear_screen();
    disable_bios_cursor();
    
    // BASE STATEMENT LOCALIZATION
    if (localization.base_state == 1 && localization.en_state == 0 && localization.ru_state == 0) {
      base_loc_system_answer();
    }
    // EN STATEMENT LOCALIZATION
    else if (localization.en_state == 1 && localization.base_state == 0 && localization.ru_state == 0) {
      en_loc_system_answer();
    }
    /* RU STATEMENT LOCALIZATION
    else if (localization.ru_state == 1 && localization.base_state == 0 && localization.en_state == 0) {
      ru_loc_system_answer();
    }
    */
    
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
