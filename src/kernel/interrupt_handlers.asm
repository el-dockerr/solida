; filepath: /home/skalski/local-dev/pocs/solida64/src/kernel/interrupt_handlers.asm[BITS 32]

global syscall_handler
global isr0
global isr1

extern handle_syscall

; Syscall handler
syscall_handler:
    push ebp
    mov ebp, esp
    
    ; Save registers
    pushad
    
    ; Call C handler with syscall number and arguments
    push edx    ; arg3
    push ecx    ; arg2
    push ebx    ; arg1
    push eax    ; syscall number
    call handle_syscall
    add esp, 16 ; Clean up stack
    
    ; Restore registers
    popad
    
    pop ebp
    iret

; Exception handlers
isr0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

isr_common_stub:
    ; Common exception handling code
    ; ...
    iret