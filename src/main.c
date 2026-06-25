// SPDX-License-Identifier: GPL-3.0-or-later

/*
    Vale only supports the x86_64 architecture.

    Compiling to a different architecture will result
    in unattended side effects, or completely unable to boot.
*/
#if defined(__x86_64__)
    #include "arch/x86_64/gdt.h"
    #include "arch/x86_64/idt.h"
    #include "arch/x86_64/pic.h"
    #include "arch/x86_64/pit.h"
#else
    #error "Unsupported architecture. Compile for x86_64."
#endif

#include "drivers/memory/pmm.h"
#include "drivers/memory/vmm.h"
#include "drivers/memory/heap.h"
#include "drivers/fb.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

void _start(void) {
    __asm__ volatile("cli"); // disable interrupts; no idt yet

    remap_pic(32, 40);

    init_gdt();
    init_idt();

    init_fb(); // initialize framebuffer early for early logging
    clear(0x000000);

    pit_init(100);

    inb(0x60);

    outb(0x21, 0xFC); // unmask irq0 & irq1 from PIC

    init_pmm();
    init_vmm();
    init_heap();

    printf("\n", 0x000000);

    printf("Welcome to Vale ", 0xFFFFFF);
    printf("0.1.0-alpha.1!\n", 0xFFFFFF);
    printf("Copyright (c) 2026 Luna Dalenuit and contributers, ", 0xCC00DD);
    printf("GNU General Public License v3.0-or-later.\n", 0xFF2200);

    __asm__ volatile("sti"); // enable interrupts; idt setup, booting done :D

    for (;;) __asm__("hlt");
}