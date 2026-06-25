// SPDX-License-Identifier: GPL-3.0-or-later

#include "pic.h"

#define PIC1_CMD 0x20
#define PIC1_DAT 0x21
#define PIC2_CMD 0xA0
#define PIC2_DAT 0xA1

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0); 
}

void remap_pic(uint8_t offset1, uint8_t offset2) {
    outb(PIC1_CMD, 0x11);
    io_wait();
    outb(PIC2_CMD, 0x11);
    io_wait();

    outb(PIC1_DAT, offset1);
    io_wait();
    outb(PIC2_DAT, offset2);
    io_wait();

    outb(PIC1_DAT, 4);
    io_wait();
    
    outb(PIC2_DAT, 2);
    io_wait();

    outb(PIC1_DAT, 0x01);
    io_wait();
    outb(PIC2_DAT, 0x01);
    io_wait();

    outb(PIC1_DAT, 0xFF);
    outb(PIC2_DAT, 0xFF);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20);
    }
    outb(PIC1_CMD, 0x20);
}