; ███╗░░░███╗██╗██████╗░███╗░░██╗██╗░██████╗░██╗░░██╗████████╗
; ████╗░████║██║██╔══██╗████╗░██║██║██╔════╝░██║░░██║╚══██╔══╝
; ██╔████╔██║██║██║░░██║██╔██╗██║██║██║░░██╗░███████║░░░██║░░░
; ██║╚██╔╝██║██║██║░░██║██║╚████║██║██║░░╚██╗██╔══██║░░░██║░░░
; ██║░╚═╝░██║██║██████╔╝██║░╚███║██║╚██████╔╝██║░░██║░░░██║░░░
; ╚═╝░░░░░╚═╝╚═╝╚═════╝░╚═╝░░╚══╝╚═╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░

; ============================================
; MULTIBOOT HEADER (FOR LIMINE)
; ============================================
section .multiboot
    align 4
    dd 0x1BADB002
    dd 0x03
    dd -(0x1BADB002 + 0x03);
    
; ============================================
; KERNEL MAIN CODE
; ============================================
;org 0x100000
section .text
bits 32

global start
extern kernel_c

extern handle_enter
extern console_putchar
extern console_backspace
global setup_idt
global setup_pic
global setup_pit
global irq0_handler
global irq1_handler
    
; ============================================
; START
; ============================================
start:
    mov byte [0xb8000], 'K'
    mov byte [0xb8001], 0x0C
    mov byte [0xb8002], 'E'
    mov byte [0xb8003], 0x0C
    mov byte [0xb8004], 'R'
    mov byte [0xb8005], 0x0C
    mov byte [0xb8006], 'N'
    mov byte [0xb8007], 0x0C
    mov byte [0xb8008], 'L'
    mov byte [0xb8009], 0x0C
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov esp, 0x9F000
    mov ebp, esp
    
    call kernel_c
    
    cli
    hlt

    mov byte [0xb8000], 'O'
    mov byte [0xb8001], 0x0A
    mov byte [0xb8000 + 2], 'K'
    mov byte [0xb8000 + 3], 0x0A

    call setup_idt
    call setup_pic
    call setup_pit

    mov byte [0xb8000 + 14], 'S'
    mov byte [0xb8000 + 15], 0x04

    sti
    
    jmp main_loop
setup_idt:
    cli
    
    mov edi, idt
    mov ecx, 256 * 8
    mov eax, 0
    rep stosb
    
    mov ecx, 256
    mov ebx, 0
.set_stubs:
    mov eax, isr_stub
    call set_idt_entry
    inc ebx
    loop .set_stubs
    
    mov eax, irq0_handler
    mov ebx, 32
    call set_idt_entry
    
    mov eax, irq1_handler
    mov ebx, 33
    call set_idt_entry
    
    lidt [idt_descriptor]
    
    ret
set_idt_entry:
    push ebx
    push eax
    
    shl ebx, 3
    add ebx, idt
    
    pop eax
    mov [ebx], ax
    shr eax, 16
    mov [ebx + 6], ax
    
    mov word [ebx + 2], 0x08
    mov word [ebx + 4], 0x8E00
    
    pop ebx
    ret

setup_pic:
    in al, 0x21
    mov [pic_mask_master], al
    in al, 0xA1
    mov [pic_mask_slave], al

    mov al, 0x11
    out 0x20, al
    mov al, 0x20
    out 0x21, al
    mov al, 0x04
    out 0x21, al
    mov al, 0x01
    out 0x21, al

    mov al, 0x11
    out 0xA0, al
    mov al, 0x28
    out 0xA1, al
    mov al, 0x02
    out 0xA1, al
    mov al, 0x01
    out 0xA1, al

    mov al, 0xFC
    out 0x21, al
    mov al, 0xFF
    out 0xA1, al
    
    ret
setup_pit:
    mov al, 0x36
    out 0x43, al
    
    mov ax, 11931
    out 0x40, al
    mov al, ah
    out 0x40, al
    
    ret
isr_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    
    mov al, 0x20
    out 0x20, al
    
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
irq0_handler:
    pusha
    push ds
    push es

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    
    inc dword [timer_ticks]
    
    ;push dword [timer_ticks]
    ;call print_decimal
    ;add esp, 4
    
    mov al, 0x20
    out 0x20, al
    
    pop es
    pop ds
    popa
    iret
irq1_handler:
    pusha
    push ds
    push es
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    
    in al, 0x60
    mov bl, al
    
    test bl, 0x80
    jnz irq1_handler_key_release
    
    cmp bl, 0x0E
    je irq1_handler_backspace
    cmp bl, 0x1C
    je irq1_handler_enter
    cmp bl, 0x2A
    je irq1_handler_shift_press
    cmp bl, 0x36
    je irq1_handler_shift_press
    
    xor eax, eax
    mov al, bl
    mov esi, ascii_scancode
    add esi, eax
    mov al, [esi]
    
    test al, al
    jz irq1_handler_key_done
    
    cmp byte [shift_pressed], 1
    jne irq1_handler_no_shift
    
    cmp al, '='
    jne irq1_handler_not_equal
    mov al, '+'
    jmp irq1_handler_no_shift
    
irq1_handler_not_equal:
    cmp al, 'a'
    jb irq1_handler_no_shift
    cmp al, 'z'
    ja irq1_handler_no_shift
    sub al, 32
    
irq1_handler_no_shift:
    push eax
    call console_putchar
    add esp, 4
    jmp irq1_handler_key_done

irq1_handler_backspace:
    call console_backspace
    jmp irq1_handler_key_done

irq1_handler_enter:
    call handle_enter
    jmp irq1_handler_key_done

irq1_handler_shift_press:
    mov byte [shift_pressed], 1
    jmp irq1_handler_key_done

irq1_handler_key_release:
    and bl, 0x7F
    cmp bl, 0x2A
    je irq1_handler_shift_release
    cmp bl, 0x36
    je irq1_handler_shift_release
    jmp irq1_handler_key_done

irq1_handler_shift_release:
    mov byte [shift_pressed], 0

irq1_handler_key_done:
    mov al, 0x20
    out 0x20, al
    
    pop es
    pop ds
    popa
    iret
.flush_done:
    mov al, 0x20
    out 0x20, al
    
    pop es
    pop ds
    popa
    iret
print_hex_byte:
    push ebp
    mov ebp, esp
    pusha
    
    mov eax, [ebp+8]
    and eax, 0xFF
    
    mov edi, 0xb8000 + 160*2 + 80*2 - 10
    
    mov ebx, eax
    shr ebx, 4
    and ebx, 0x0F
    add bl, '0'
    cmp bl, '9' + 1
    jl print_hex_byte_first_ok
    add bl, 'A' - '9' - 1
print_hex_byte_first_ok:
    mov [edi], bl
    mov byte [edi+1], 0x0E
    
    mov ebx, eax
    and ebx, 0x0F
    add bl, '0'
    cmp bl, '9' + 1
    jl print_hex_byte_second_ok
    add bl, 'A' - '9' - 1
print_hex_byte_second_ok:
    mov [edi+2], bl
    mov byte [edi+3], 0x0E
    
    popa
    pop ebp
    ret
print_decimal:
    push ebp
    mov ebp, esp
    pusha
    
    mov eax, [ebp+8]
    mov edi, 0xb8000 + 160*1 + 70*2
    mov ecx, 0
    mov ebx, 10
    
    mov word [edi], 0x0720
    mov word [edi+2], 0x0720
    mov word [edi+4], 0x0720
    mov word [edi+6], 0x0720
    mov word [edi+8], 0x0720
    
.divide:
    xor edx, edx
    div ebx
    push edx
    inc ecx
    test eax, eax
    jnz .divide
    
.show:
    pop eax
    add al, '0'
    mov [edi], al
    mov byte [edi+1], 0x0F
    add edi, 2
    loop .show
    
    popa
    pop ebp
    ret
clear_char:
    pusha
    
    mov ebx, [cursor_y]
    imul ebx, 160
    mov ecx, [cursor_x]
    shl ecx, 1
    add ebx, ecx
    add ebx, 0xb8000
    
    mov word [ebx], 0x0F20
    
    popa
    ret
    
putchar:
    pusha
    
    mov ebx, [cursor_y]
    imul ebx, 160
    mov ecx, [cursor_x]
    shl ecx, 1
    add ebx, ecx
    add ebx, 0xb8000
    
    mov [ebx], al
    mov byte [ebx+1], 0x0F
    
    inc dword [cursor_x]
    cmp dword [cursor_x], 80
    jl .done
    
    mov dword [cursor_x], 0
    inc dword [cursor_y]
    cmp dword [cursor_y], 25
    jl .done
    
    call scroll_screen
    mov dword [cursor_y], 24
    
.done:
    popa
    ret
scroll_screen:
    pusha
    
    mov esi, 0xb8000 + 160
    mov edi, 0xb8000
    mov ecx, 80*24
    rep movsw
    
    mov edi, 0xb8000 + 160*24
    mov ecx, 80
    mov eax, 0x0720
    rep stosw
    
    popa
    ret
draw_cursor:
    pusha
    
    mov ebx, [cursor_y]
    imul ebx, 160
    mov ecx, [cursor_x]
    shl ecx, 1
    add ebx, ecx
    add ebx, 0xb8000
    
    mov byte [ebx], ' '    ; CURSOR and BACKSPACE CHAR
    mov byte [ebx+1], 0x0F
    
    popa
    ret
main_loop:
    call draw_cursor
    hlt
    
    jmp main_loop
section .data

pic_mask_master: db 0
pic_mask_slave: db 0

timer_ticks: dd 0
shift_pressed: db 0

extern min_x
extern min_y
extern cursor_x
extern cursor_y

max_x: dd 60

extern min_x
extern min_y

align 8
idt: times 256*8 db 0

idt_descriptor:
    dw (256*8 - 1)
    dd idt

ascii_scancode:
    db 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0
    db 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's'
    db 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', "'", '`', 0, '\', 'z', 'x', 'c', 'v'
    db 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0
    times 128-($-ascii_scancode) db 0

times 512*10-($-$$) db 0
