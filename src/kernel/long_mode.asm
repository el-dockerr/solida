[BITS 32]
global setup_long_mode
extern kernel_main

section .text
setup_long_mode:
    ; Print "32" marker
    mov dword [0xb8000], 0x2f322f33   ; "32"

    ; Check for CPUID
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz no_long_mode    ; Remove the dot

    ; Setup paging
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
    mov eax, 0x1000
    mov cr3, eax
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Load 64-bit GDT
    lgdt [GDT64.Pointer]
    
    ; Far jump to 64-bit code
    jmp 0x08:long_mode_start    ; Remove the dot

no_long_mode:    ; Remove the dot
    mov dword [0xb8000], 0x4f524f45  ; "ER"
    hlt

section .rodata
align 8
GDT64:
    dq 0                ; Null descriptor
    dq 0x00209A0000000000 ; 64-bit code segment
    dq 0x0000920000000000 ; 64-bit data segment
.Pointer:
    dw $ - GDT64 - 1   ; GDT size
    dq GDT64           ; GDT address

[BITS 64]
long_mode_start:    ; Remove the dot
    ; Print "64" marker
    mov rax, 0x2f342f36        ; "64"
    mov qword [0xb8000], rax

    ; Setup segment registers
    mov ax, 0x10              ; Data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Call kernel main
    call kernel_main