#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// IDT Gate Types
#define IDT_INTERRUPT_GATE 0x8E
#define IDT_TRAP_GATE     0x8F

// Structure for IDT entry
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// Structure for IDTR
struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void interrupts_init(void);
void setup_idt(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif