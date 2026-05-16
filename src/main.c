// SPDX-License-Identifier: GPL-3.0-or-later

/*
    Vale only supports the x86_64 architecture.

    Compiling to a different architecture will result
    in unattended side effects, or completely unable to boot.
*/
#if defined(__x86_64__)
    #include "arch/x86_64/gdt.h"
#else
    #error "Unsupported architecture. Compile for x86_64."
#endif

#include "drivers/memory/pmm.h"
#include "drivers/memory/vmm.h"
#include "drivers/memory/heap.h"
#include "drivers/fb.h"

void _start(void) {
    __asm__ volatile("cli"); // disable interrupts; no idt yet

    init_gdt();

    init_fb(); // initialize framebuffer early for early logging
    clear(0x000000);

    init_pmm();
    init_vmm();
    init_heap();

    printf("\n", 0x000000);

    printf("Welcome to Vale ", 0xFFFFFF);
    printf("0.1.0-alpha.1!\n", 0xFFFFFF);
    printf("Copyright (c) 2026 Aerilyn Vale and contributers, ", 0xCC00DD);
    printf("GNU General Public License v3.0-or-later.\n", 0xFF2200);


    // heap testing
    
    for (int i = 0; i < 10000; i++) {
        void *x = kmalloc(64);

        if (!x) {
            printf("alloc failed\n", 0xFF0000);
            break;
        }

        ((char *)x)[0] = 0xAA;
    }

    void *x = kmalloc(1);

    if (((uint64_t)x & 0xF) != 0) {
        printf("bad alignment\n", 0xFF0000);
    }

    printf("If you see this, heap probably works. :3", 0x00FF00);

    for (;;) __asm__("hlt");
}