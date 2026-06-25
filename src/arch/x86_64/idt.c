// SPDX-License-Identifier: GPL-3.0-or-later

#include "idt.h"
#include "../../drivers/fb.h"

#define IDT_ENTRIES 256
#define IDT_INTERRUPT_GATE 0x8E

#define IRQ_BASE 32

static idt_entry_t idt[IDT_ENTRIES];
static idtr_t idtr;

extern void isr0(void);
extern void isr13(void);
extern void isr14(void);
extern void isr8(void);

extern void irq0(void);
extern void irq1(void);

static void idt_set_gate(int vector, void *handler, uint8_t flags) {
    uint64_t addr = (uint64_t)handler;

    idt[vector].offset_low = addr & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].type_attr = flags;
    idt[vector].offset_mid = (addr >> 16) & 0xFFFF;
    idt[vector].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].zero = 0;
}

static inline void lidt(idtr_t *idtr) {
    __asm__ volatile("lidt (%0)" :: "r"(idtr));
}

void init_idt(void) {
    idt_set_gate(0, isr0, IDT_INTERRUPT_GATE);
    idt_set_gate(8, isr8, IDT_INTERRUPT_GATE);
    idt_set_gate(13, isr13, IDT_INTERRUPT_GATE);
    idt_set_gate(14, isr14, IDT_INTERRUPT_GATE);

    idt_set_gate(IRQ_BASE + 0, irq0, IDT_INTERRUPT_GATE);
    idt_set_gate(IRQ_BASE + 1, irq1, IDT_INTERRUPT_GATE);

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    lidt(&idtr);
}