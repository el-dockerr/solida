[BITS 32]
global setup_long_mode
extern kernel_main

section .text
setup_long_mode:
    ; Setup initial page tables
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096*3
    rep stosd

    ; Identity map first 2MB
    mov dword [0x1000], 0x2003      ; P4[0] -> P3
    mov dword [0x2000], 0x3003      ; P3[0] -> P2
    mov dword [0x3000], 0x83        ; P2[0] -> 2MB page

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Load GDT
    lgdt [GDT64.Pointer]
    
    ; Jump to 64-bit code
    jmp 0x08:long_mode_start

section .rodata
align 8
GDT64:
    dq 0                            ; Null descriptor
    dq 0x00AF9A000000FFFF          ; 64-bit code segment
    dq 0x00AF92000000FFFF          ; 64-bit data segment
.Pointer:
    dw $ - GDT64 - 1               ; Size
    dq GDT64                       ; Address

[BITS 64]
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Call kernel
    call kernel_main