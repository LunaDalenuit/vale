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
    printf("GNU General Public License v3.0-or-later.\n\n", 0xFF2200);

    // ============================================
    //           HEAP TESTING SUITE(tm)
    // ============================================
    printf("[HEAP TEST] Starting allocator verification...\n", 0xFFFF00);

    // Test 1: Basic allocation
    void *ptr1 = kmalloc(256);
    void *ptr2 = kmalloc(512);
    void *ptr3 = kmalloc(256);

    if (ptr1 != NULL && ptr2 != NULL && ptr3 != NULL) {
        printf("  -> Test 1 Passed: Allocation successful.\n", 0x00FF00);
    } else {
        printf("  -> Test 1 Failed: Allocation returned NULL.\n", 0xFF0000);
    }

    // Test 2: Freeing and Recycling
    // Free the middle block (ptr2, size 512)
    kfree(ptr2);

    // Request a 128 byte block. It should fit right into the 512-byte hole left by ptr2.
    void *ptr4 = kmalloc(128);

    if (ptr4 == ptr2) {
        printf("  -> Test 2 Passed: Memory recycled and split correctly.\n", 0x00FF00);
    } else {
        printf("  -> Test 2 Failed: Did not recycle block from first-fit free list.\n", 0xFF0000);
    }

    // Test 3: Coalescing (Merging adjacent free blocks)
    // Currently free layout: [ptr4 (128 used)] [remaining hole (~352 free)]
    // If we free ptr4, the entire original 512 block should merge back into one solid free block.
    kfree(ptr4);
    
    // Now allocate a full 512 bytes again. If coalescing worked, it will grab the merged block.
    void *ptr5 = kmalloc(512);

    if (ptr5 == ptr2) {
        printf("  -> Test 3 Passed: Memory coalesced successfully.\n", 0x00FF00);
    } else {
        printf("  -> Test 3 Failed: Blocks failed to merge correctly.\n", 0xFF0000);
    }

    // Clean up remaining test blocks
    kfree(ptr1);
    kfree(ptr3);
    kfree(ptr5);

    printf("[HEAP TEST] Verification complete!\n\n", 0x00FFFF);
    // ============================================

    __asm__ volatile("sti"); // enable interrupts; idt setup, booting done :D

    for (;;) __asm__("hlt");
}