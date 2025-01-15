[BITS 32]
global setup_long_mode
extern kernel_main

section .text
setup_long_mode:
    ; Debug marker
    mov dword [0xb8000], 0x2f312f53   ; "S1"

    ; Clear page tables
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096*3
    rep stosd

    ; Map first 2MB for both low and high memory
    mov dword [0x1000], 0x2003      ; P4[0] -> P3
    mov dword [0x2000], 0x3003      ; P3[0] -> P2
    mov dword [0x3000], 0x83        ; P2[0] -> 2MB page with present, write, huge

    ; Debug marker
    mov dword [0xb8000], 0x2f322f53   ; "S2"

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

    ; Debug marker
    mov dword [0xb8000], 0x2f332f53   ; "S3"

    ; Load new GDT
    lgdt [GDT64.Pointer]

    ; Far jump to 64-bit code
    jmp 0x08:long_mode_start

section .rodata
align 8
GDT64:
    dq 0                            ; Null descriptor
    dq 0x00AF9A000000FFFF          ; 64-bit code segment (exec/read)
    dq 0x00AF92000000FFFF          ; 64-bit data segment (read/write)
.Pointer:
    dw $ - GDT64 - 1               ; Size
    dq GDT64                       ; Address

section .data
align 4096
stack_bottom:
    resb 16384
stack_top:

[BITS 64]
long_mode_start:
    ; Debug marker
    mov rax, 0x2f342f36           ; "64"
    mov qword [0xb8000], rax

    ; Setup stack
    mov rsp, stack_top
    mov rbp, rsp

    ; Setup segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Call kernel
    call kernel_main