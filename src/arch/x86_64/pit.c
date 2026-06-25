// SPDX-License-Identifier: GPL-3.0-or-later

#include "pit.h"

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

void pit_init(uint32_t frequency) {
    
    uint32_t divisor = 1193182 / frequency;

    outb(0x43, 0x36);                   // command byte
    outb(0x40, divisor & 0xFF);         // low byte
    outb(0x40, (divisor >> 8) & 0xFF);  // high byte
}