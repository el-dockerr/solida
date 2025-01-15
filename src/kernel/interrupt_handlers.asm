[BITS 64]

global syscall_handler
global isr0
global isr1

extern handle_syscall

; Syscall handler
syscall_handler:
    push rbp
    mov rbp, rsp
    
    ; Save registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    
    ; Call C handler with syscall number and arguments
    mov rdi, rax    ; syscall number
    mov rsi, rbx    ; arg1
    mov rdx, rcx    ; arg2
    mov rcx, rdx    ; arg3
    call handle_syscall
    
    ; Restore registers
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    pop rbp
    iretq

; Exception handlers
isr0:
    cli
    push qword 0    ; error code
    push qword 0    ; interrupt number
    jmp isr_common_stub

isr1:
    cli
    push qword 0    ; error code
    push qword 1    ; interrupt number
    jmp isr_common_stub

isr_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Add exception handling here
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    add rsp, 16     ; cleanup error code and interrupt number
    iretq