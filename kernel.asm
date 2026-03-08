[org 0x10000]
[bits 32]

bpb:
    .jmp_boot         db 0xEB, 0x3C, 0x90
    .oem_name         db "MIDNIGHT"
    .bytes_per_sector dw 512
    .sectors_per_cluster db 1
    .reserved_sectors dw 1
    .fat_count        db 2
    .root_entries     dw 224
    .total_sectors    dw 2880
    .media_descriptor db 0xF0
    .sectors_per_fat  dw 9
    .sectors_per_track dw 18
    .heads            dw 2
    .hidden_sectors   dd 0
    .large_sectors    dd 0
    
start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x20000
    
    mov edi, 0xb8000
    mov ecx, 80*25
    mov eax, 0x0720
    rep stosw

    mov byte [0xb8000], 'O'
    mov byte [0xb8001], 0x0A
    mov byte [0xb8000 + 2], 'K'
    mov byte [0xb8000 + 3], 0x0A

    mov byte [0xb8000 + 400], 'K'
    mov byte [0xb8000 + 401], 0x0F
    mov byte [0xb8000 + 402], 'i'
    mov byte [0xb8000 + 403], 0x0F
    mov byte [0xb8000 + 404], 't'
    mov byte [0xb8000 + 405], 0x0F
    mov byte [0xb8000 + 406], ' '
    mov byte [0xb8000 + 407], 0x0F
    mov byte [0xb8000 + 408], 'l'
    mov byte [0xb8000 + 409], 0x0F
    mov byte [0xb8000 + 410], 'e'
    mov byte [0xb8000 + 411], 0x0F
    mov byte [0xb8000 + 412], 't'
    mov byte [0xb8000 + 413], 0x0F
    mov byte [0xb8000 + 414], 'a'
    mov byte [0xb8000 + 415], 0x0F
    mov byte [0xb8000 + 416], 'e'
    mov byte [0xb8000 + 417], 0x0F
    mov byte [0xb8000 + 418], 't'
    mov byte [0xb8000 + 419], 0x0F
    mov byte [0xb8000 + 420], ' '
    mov byte [0xb8000 + 421], 0x0F
    mov byte [0xb8000 + 422], 'v'
    mov byte [0xb8000 + 423], 0x0F
    mov byte [0xb8000 + 424], ' '
    mov byte [0xb8000 + 425], 0x0F
    mov byte [0xb8000 + 426], 'o'
    mov byte [0xb8000 + 427], 0x0F
    mov byte [0xb8000 + 428], 'b'
    mov byte [0xb8000 + 429], 0x0F
    mov byte [0xb8000 + 430], 'l'
    mov byte [0xb8000 + 431], 0x0F
    mov byte [0xb8000 + 432], 'a'
    mov byte [0xb8000 + 433], 0x0F
    mov byte [0xb8000 + 434], 'k'
    mov byte [0xb8000 + 435], 0x0F
    mov byte [0xb8000 + 436], 'a'
    mov byte [0xb8000 + 437], 0x0F
    mov byte [0xb8000 + 438], 'h'
    mov byte [0xb8000 + 439], 0x0F
    mov byte [0xb8000 + 440], ' '
    mov byte [0xb8000 + 441], 0x0F
    mov byte [0xb8000 + 442], 'i'
    mov byte [0xb8000 + 443], 0x0F
    mov byte [0xb8000 + 444], ' '
    mov byte [0xb8000 + 445], 0x0F
    mov byte [0xb8000 + 446], 'm'
    mov byte [0xb8000 + 447], 0x0F
    mov byte [0xb8000 + 448], 'e'
    mov byte [0xb8000 + 449], 0x0F
    mov byte [0xb8000 + 450], 'c'
    mov byte [0xb8000 + 451], 0x0F
    mov byte [0xb8000 + 452], 'h'
    mov byte [0xb8000 + 453], 0x0F
    mov byte [0xb8000 + 454], 't'
    mov byte [0xb8000 + 455], 0x0F
    mov byte [0xb8000 + 456], 'a'
    mov byte [0xb8000 + 457], 0x0F
    mov byte [0xb8000 + 458], 'e'
    mov byte [0xb8000 + 459], 0x0F
    mov byte [0xb8000 + 460], 't'
    mov byte [0xb8000 + 461], 0x0F

    call setup_idt
    call setup_pic
    call setup_pit

    mov byte [0xb8000 + 14], 'S'
    mov byte [0xb8000 + 15], 0x04

    sti
    
    jmp main_loop

read_sector:
    pusha
    push es
    
    xor dx, dx
    div word [bpb.sectors_per_track]
    inc dl
    mov cl, dl
    
    xor dx, dx
    div word [bpb.heads]
    mov dh, dl
    mov ch, al
    shl ah, 6
    or cl, ah
    
    mov ax, 0x0201
    mov dl, 0x80
    int 0x13
    jc .error
    
    pop es
    popa
    ret
    
.error:
    mov byte [0xb8000 + 480], 'E'
    mov byte [0xb8000 + 481], 0x04
    jmp $
    
write_sector:
    pusha
    push es
    
    xor dx, dx
    div word [bpb.sectors_per_track]
    inc dl
    mov cl, dl
    
    xor dx, dx
    div word [bpb.heads]
    mov dh, dl
    mov ch, al
    shl ah, 6
    or cl, ah
    
    mov ax, 0x0301
    mov dl, 0x80
    int 0x13
    jc .error
    
    pop es
    popa
    ret
    
.error:
    mov byte [0xb8000 + 482], 'W'
    mov byte [0xb8000 + 483], 0x04
    jmp $
    
read_root_directory:
    pusha
    
    mov ax, [bpb.reserved_sectors]
    mov bl, [bpb.fat_count]
    xor bh, bh
    mul bx
    mov [root_start], ax
    
    mov bx, [bpb.root_entries]
    shl bx, 5
    add bx, 511
    shr bx, 9
    mov [root_sectors], bx
    
    push es
    mov bx, 0x2000
    mov es, bx
    xor bx, bx
    
    mov ax, [root_start]
    mov cx, [root_sectors]
    
.read_loop:
    push cx
    push ax
    push bx
    call read_sector
    pop bx
    pop ax
    pop cx
    
    inc ax
    add bx, 512
    loop .read_loop
    
    pop es
    popa
    ret
    
read_fat:
    pusha
    
    mov ax, [bpb.reserved_sectors]
    mov [fat_start], ax
    
    mov cx, [bpb.sectors_per_fat]
    
    push es
    mov bx, 0x3000
    mov es, bx
    xor bx, bx
    
.fat_read_loop:
    push cx
    push ax
    push bx
    call read_sector
    pop bx
    pop ax
    pop cx
    
    inc ax
    add bx, 512
    loop .fat_read_loop
    
    pop es
    popa
    ret
    
get_next_cluster:
    push bx
    push cx
    push ds
    
    mov cx, 0x3000
    mov ds, cx
    
    mov bx, ax
    shr bx, 1
    add bx, ax
    
    mov cx, [bx]
    
    test ax, 1
    jnz .odd_cluster
    
    and cx, 0x0FFF
    jmp .done
    
.odd_cluster:
    shr cx, 4
    
.done:
    mov ax, cx
    pop ds
    pop cx
    pop bx
    ret
    
load_file:
    pusha
    push ax
    
    mov ax, [bpb.reserved_sectors]
    mov bl, [bpb.fat_count]
    xor bh, bh
    mul bx
    add ax, [bpb.reserved_sectors]
    add ax, [root_sectors]
    mov [data_start], ax
    
    pop ax
    
.load_cluster: 
    push ax
    sub ax, 2
    mov bl, [bpb.sectors_per_cluster]
    xor bh, bh
    mul bx
    add ax, [data_start]
    mov bx, ax
    
    pop ax
    push ax
    push es
    push di
    mov ax, bx
    call read_sector
    pop di
    pop es
    pop ax
    
    add di, 512
    
    push ax
    call get_next_cluster
    pop bx
    
    cmp ax, 0x0FF8
    jl .load_cluster
    
    popa
    ret

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
    
    push dword [timer_ticks]
    call print_decimal
    add esp, 4
    
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
    
    push eax
    call print_hex_byte
    add esp, 4
    
    test bl, 0x80
    jnz .key_release
    
    cmp bl, 0x0E
    je .backspace
    cmp bl, 0x1C
    je .enter
    cmp bl, 0x2A
    je .shift_press
    cmp bl, 0x36
    je .shift_press
    
    xor eax, eax
    mov al, bl
    mov esi, ascii_scancode
    add esi, eax
    mov al, [esi]
    
    test al, al
    jz .key_done
    
    cmp byte [shift_pressed], 1
    jne .no_shift
    
    cmp al, '='
    jne .not_equal
    mov al, '+'
    jmp .no_shift
    
.not_equal:
    cmp al, 'a'
    jb .no_shift
    cmp al, 'z'
    ja .no_shift
    sub al, 32
    
.no_shift:
    call putchar
    jmp .key_done

.backspace:
    cmp dword [cursor_x], 0
    je .check_y_backspace
    
    dec dword [cursor_x]
    call clear_char
    jmp .key_done
    
.check_y_backspace:
    cmp dword [cursor_y], 0
    je .key_done
    
    dec dword [cursor_y]
    mov dword [cursor_x], 79
    call clear_char
    jmp .key_done

.enter:
    mov dword [cursor_x], 0
    inc dword [cursor_y]
    
    cmp dword [cursor_y], 25
    jl .enter_done
    call scroll_screen
    mov dword [cursor_y], 24
    
.enter_done:
    jmp .key_done

.shift_press:
    mov byte [shift_pressed], 1
    jmp .key_done

.key_release:
    and bl, 0x7F
    cmp bl, 0x2A
    je .shift_release
    cmp bl, 0x36
    je .shift_release
    jmp .key_done

.shift_release:
    mov byte [shift_pressed], 0

.key_done:
    in al, 0x64
    test al, 1
    jz .flush_done
    in al, 0x60
    jmp .key_done
.flush_done:
    
    ; EOI
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
    
    mov edi, 0xb8000 + 160*2 + 70*2
    
    mov ebx, eax
    shr ebx, 4
    and ebx, 0x0F
    add bl, '0'
    cmp bl, '9' + 1
    jl .first_ok
    add bl, 'A' - '9' - 1
.first_ok:
    mov [edi], bl
    mov byte [edi+1], 0x0E
    
    mov ebx, eax
    and ebx, 0x0F
    add bl, '0'
    cmp bl, '9' + 1
    jl .second_ok
    add bl, 'A' - '9' - 1
.second_ok:
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
    
    mov word [ebx], 0x0720
    
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
    
    mov byte [ebx], '_'
    mov byte [ebx+1], 0x0F
    
    popa
    ret

main_loop:
    call draw_cursor
    hlt
    jmp main_loop

section .data

root_start: dw 0
root_sectors: dw 0
fat_start: dw 1
data_start: dw 0

pic_mask_master: db 0
pic_mask_slave: db 0

timer_ticks: dd 0
cursor_x: dd 5
cursor_y: dd 10
shift_pressed: db 0

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
