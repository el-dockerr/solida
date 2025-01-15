#include "interrupts.h"
#include "syscalls.h"
#include "io.h"

#define IDT_ENTRIES 256
#define KERNEL_CS 0x08

struct idt_entry idt[IDT_ENTRIES];
struct idtr idtr;

// Assembly interrupt handlers
extern void isr0(void);
extern void isr1(void);
// ... other ISRs
extern void syscall_handler(void);

static void pic_remap(void) {
    // Initialize PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void interrupts_init(void) {
    // Set up IDT pointer
    idtr.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtr.base = (uint32_t)&idt;

    // Clear IDT and memory
    for(int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)isr0, KERNEL_CS, IDT_INTERRUPT_GATE);
    }

    // Set up exception handlers with proper error handling
    idt_set_gate(0, (uint32_t)isr0, KERNEL_CS, IDT_INTERRUPT_GATE);
    idt_set_gate(1, (uint32_t)isr1, KERNEL_CS, IDT_INTERRUPT_GATE);

    // Remap PIC first
    pic_remap();
    
    // Load IDT
    asm volatile("lidt %0" : : "m"(idtr));
    
    // Clear any pending interrupts
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
    
    // Enable interrupts last
    asm volatile("sti");
}