// ============================================
// MIDNIGHT // REWRITE
// [Build: 14.04.2026]
// ============================================
#include <stddef.h>
#include <stdint.h>

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

// ============================================
// GLOBAL
// [Build: Not Used]
// ============================================
unsigned char* video = (unsigned char*)0xB8000;
int cursor_x = 0;
int cursor_y = 0;
int min_x = 0;
int min_y = 0;
int max_x = 60;

static char input_buffer[256];
static int input_len = 0;

// ============================================
// VIDEO FUNCTIONS
// [Build: Not Used]
// ============================================
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

void print_int(int num) {
    char buf[12];
    int i = 0;
    
    if (num == 0) {
        print("0", 0x0F);
        return;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        i--;
        char c = buf[i];
        print(&c, 0x0F);
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
// [Build: Not Used]
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
    print("No language selected. Using base_state parameter from file: FILE_FILE_FILE.p\n", 0x07);
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
  print("Загружаем: C\n", 0x0F);
  print("Инициализация систему...\n", 0x0F);
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
// [Build: 14.04.2026]
// =========================
struct rights_array {
  char* APDR;
  char* AP;
  char* A;
  char* U;
  
  char* dreamy;
  char* user;
  char* current;
};

struct rights_array rights;

void rights_init(void) {
  rights.APDR = "Alpha-Prime [DIGITAL//REWRITE]";  // only for dev
  rights.AP = "Alpha-Prime";  // for Data-Rewrite
  rights.A = "Alpha";  // for User
  rights.U = "User";  // for User
  
  rights.dreamy = rights.APDR;  // its me (dev)
  rights.user = rights.U;   // user
  
  rights.current = rights.dreamy;
  
  if (rights.current == rights.user || rights.current == rights.U || rights.current == rights.AP || rights.current == rights.A || rights.current == rights.APDR || rights.current == rights.dreamy) {
    print("Rights: ", 0x0F);
    print(rights.current, 0x0F);
    print("\n", 0x0F);
    print("Access: Full access\n", 0x0F);  // display, change this when i write what i can do with rights [list]
  };
  
  /*
  AP - System, Kernel, [dreamy], Code Enthusiast
  A - User max rights
  U - User base rights
  */
};

void lng_swap_base(void) {
  if (localization.base_state == 0) {
    localization.base_state = 1;
    localization.en_state = 0;
    localization.ru_state = 0;
    print("BASE [EN] lang enabled\n", 0x0A);
    print("Write: 'rboot' to reboot PC and enable new language.\n", 0x0F);
  }
  else {
    print("BASE [EN] lang already enabled!", 0x0C);
  }
}

void lng_swap_en(void) {
  if (localization.en_state == 0) {
    localization.base_state = 0;
    localization.en_state = 1;
    localization.ru_state = 0;
    print("[EN] lang enabled\n", 0x0A);
    print("Write: 'rboot' to reboot PC and enable new language.\n", 0x0F);
  }
  else {
    localization.base_state = 1;
    localization.en_state = 0;
    localization.ru_state = 0;
    print("[EN] lang disabled!\n", 0x0C);
    print("Using base_state, because midnight doesnt found any langs enabled.\n", 0x0E);
    print("Write: 'rboot' to reboot PC and enable new language.\n", 0x0F);
  }
}

void lng_state(void) {
  if (localization.base_state == 1) {
    print("BASE [EN] = ", 0x0A);
    print_int(localization.base_state);
    print("\n", 0x0A);
  }
  if (localization.base_state != 1) {
    print("BASE [EN] = ", 0x0C);
    print_int(localization.base_state);
    print("\n", 0x0A);
  }
  if (localization.en_state == 1) {
    print("[EN] = ", 0x0A);
    print_int(localization.en_state);
    print("\n", 0x0A);
  }
  if (localization.en_state != 1) {
    print("[EN] = ", 0x0C);
    print_int(localization.en_state);
    print("\n", 0x0A);
  }
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
    }
    return NULL;
}
  
int parse_color(char* args, char** new_args) {
    char* color_pos = strstr(args, "..color ");
    
    if (!color_pos) {
        *new_args = args;
        return 0x0F;
    }
    
    *color_pos = '\0';
    *new_args = args;
    
    char* color_name = color_pos + 8;
    
    char* space = color_name;
    while (*space && *space != ' ' && *space != '\n') {
        space++;
    }
    if (*space) {
        *space = '\0';
    }
    
    if (strcmp(color_name, "lime") == 0) return 0x0A;
    if (strcmp(color_name, "red") == 0) return 0x0C;
    if (strcmp(color_name, "white") == 0) return 0x0F;
    if (strcmp(color_name, "yellow") == 0) return 0x0E;
    if (strcmp(color_name, "cyan") == 0) return 0x0B;
    if (strcmp(color_name, "blue") == 0) return 0x09;
    if (strcmp(color_name, "magenta") == 0) return 0x0D;
    if (strcmp(color_name, "gray") == 0) return 0x08;
    if (strcmp(color_name, "green") == 0) return 0x02;
    
    return 0x0F;
}

void echo(char* args) {
    if (args[0] == '\0') {
        print("\n", 0x0F);
        return;
    }
    
    char* text_to_print;
    int color = parse_color(args, &text_to_print);
    
    print(text_to_print, color);
    print("\n", 0x0F);
}

// ==============================
// [KERNEL PARAMETER BLOCK]
// [Build: 14.04.2026]
// ==============================
struct KERNEL_ARRAY {
    int kernel_after;
};

struct KERNEL_ARRAY kernel_array;

void KERNEL_FAILURE(void) {
    kernel_array.kernel_after = 0;
    
    print("KERNEL FAILURE.\n", 0x0C);
    print("Host: kernel.asm in /workspace/Midnight/AP/Core/Kernel.asm\n", 0x0C);
    print("0x024001678", 0x0F);
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

void execute_command(const char* input) {
    char cmd[256];
    char args[256];
    int i = 0;
    
    while (input[i] && input[i] != ' ') {
        cmd[i] = input[i];
        i++;
    }
    cmd[i] = '\0';
    
    if (input[i] == ' ') {
        int j = 0;
        i++;
        while (input[i]) {
            args[j++] = input[i++];
        }
        args[j] = '\0';
    } else {
        args[0] = '\0';
    }
    
    if (strcmp(cmd, "hlp") == 0) {
        print("hlp - Command list\n", 0x0F);
        print("clr - Clear screen\n", 0x0F);
        print("inf - Information\n", 0x0F);
        print("rights - Your rights\n", 0x0F);
        print("poff - Power off\n", 0x0F);
        print("rboot - Reboot\n", 0x0F);
        print("echo <text> - Print text\n", 0x0F);
        print("echo <text> ..color <color> - Print text with color\n", 0x0F);
        print("lng swap base - Switch to BASE (English)\n", 0x0F);
        print("lng swap en - Switch to English\n", 0x0F);
        //print("lng swap ru - Switch to Russian\n", 0x0F);
        print("lng state - Show language state\n", 0x0F);
    }
    else if (strcmp(cmd, "clr") == 0) {
        clear_screen();
        console_prefix();
        min_x = cursor_x;
        min_y = cursor_y;
    }
    else if (strcmp(cmd, "inf") == 0) {
        print("MidnightOS v1.9\n", 0x0F);
        print("Build: 09.04.2026\n", 0x0F);
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
    else if (strcmp(cmd, "echo") == 0) {
        echo(args);
    }
    else if (strcmp(args, "kernel-after 0") == 0) {
        kernel_array.kernel_after = 0;
        clear_screen();
        KERNEL_FAILURE();
    }
    else if (strcmp(cmd, "lng") == 0) {
        if (strcmp(args, "swap base") == 0) {
            lng_swap_base();
        }
        else if (strcmp(args, "swap en") == 0) {
            lng_swap_en();
        }
        /*
        else if (strcmp(args, "swap ru") == 0) {
            lng_swap_ru();
        }
        */
        else if (strcmp(args, "state") == 0) {
            lng_state();
        }
        else {
            print("Usage: lng [base|en|ru|state]\n", 0x0C);
        }
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

// Not a code.

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

// ============================================
// EXTERNS
// [Build: Not Used]
// ============================================
extern void setup_idt();
extern void setup_pic();
extern void setup_pit();

// ============================================
// MAIN FUNCTION
// [Build: Not Used]
// ============================================
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
    
    // Setup
    setup_idt();
    setup_pic();
    setup_pit();
    asm volatile("sti");
    
    // Position for console
    cursor_x = 0;
    cursor_y = 14;
    console_prefix();
    
    min_x = cursor_x;  // position after cmd (for backspace)
    min_y = cursor_y;
    
    while(1) {
        asm volatile("hlt");
    }
}
