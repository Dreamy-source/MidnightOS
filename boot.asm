[org 0x7c00]
[bits 16]

mov [boot_drive], dl

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x9000
mov bp, sp

mov si, kernel_loading_msg
call print_string

kernel_load:
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 10
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    mov si, kernel_success_msg
    call print_string

    ; переход в Protected Mode
    cli
    lgdt [gdt_descriptor]
    in al, 0x92
    or al, 2
    out 0x92, al
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_error:
    mov si, kernel_error_msg
    call print_string
    jmp halt

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

halt:
    cli
    hlt
    jmp halt

boot_drive: db 0
kernel_loading_msg db 'Loading kernel...', 0x0d, 0x0a, 0
kernel_success_msg db 'Kernel loaded!', 0x0d, 0x0a, 0
kernel_error_msg db 'Disk error! System halted', 0x0d, 0x0a, 0

; GDT
gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw (gdt_end - gdt_start) - 1
    dd gdt_start

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov eax, 0xb8000
    mov byte [eax], 'P'
    mov byte [eax+1], 0x04

    ; Проверка байта из ядра
    mov eax, 0x10000
    mov bl, [eax]
    mov [0xb8002], bl
    mov byte [0xb8003], 0x02

    jmp 0x08:0x10000

times 510-($-$$) db 0
dw 0xaa55
