// SPDX-License-Identifier: GPL-3.0-or-later

#include "exceptions.h"
#include "../../drivers/fb.h"

static const char *exception_name(uint64_t vector) {
    switch (vector) {
        case 0:  return "Divide By Zero";
        case 1:  return "Debug";
        case 2:  return "NMI";
        case 3:  return "Breakpoint";
        case 4:  return "Overflow";
        case 5:  return "Bound Range Exceeded";
        case 6:  return "Invalid Opcode";
        case 7:  return "Device Not Available";
        case 8:  return "Double Fault";
        case 10: return "Invalid TSS";
        case 11: return "Segment Not Present";
        case 12: return "Stack Segment Fault";
        case 13: return "General Protection Fault";
        case 14: return "Page Fault";
        case 16: return "x87 Floating Point";
        case 17: return "Alignment Check";
        case 18: return "Machine Check";
        case 19: return "SIMD Floating Point";
        default: return "Unknown";
    }
}

static void decode_page_fault(uint64_t error) {
    printf("\nPage Fault Analysis:\n", 0xFFFF00);

    printf("Present: ", 0xFFFFFF);
    printf((error & 1) ? "yes\n" : "no\n", 0xFFFFFF);

    printf("Write: ", 0xFFFFFF);
    printf((error & 2) ? "yes\n" : "no\n", 0xFFFFFF);

    printf("User: ", 0xFFFFFF);
    printf((error & 4) ? "yes\n" : "no\n", 0xFFFFFF);

    printf("Reserved Bit: ", 0xFFFFFF);
    printf((error & 8) ? "yes\n" : "no\n", 0xFFFFFF);

    printf("Instruction Fetch: ", 0xFFFFFF);
    printf((error & 16) ? "yes\n" : "no\n", 0xFFFFFF);
}

void exception_handler(interrupt_frame_t *frame) {
    clear(0x220000);

    printf("\n========== KERNEL EXCEPTION ==========\n", 0xFF0000);

    printf("Exception: ", 0xFF0000);
    printf((char *)exception_name(frame->vector), 0xFF0000);
    printf("\n", 0xFF0000);

    printf("Vector: ", 0xFF0000);
    print_hex64(frame->vector, 0xFF0000);
    printf("\n", 0xFF0000);

    printf("Error Code: ", 0xFF0000);
    print_hex64(frame->error, 0xFF0000);
    printf("\n", 0xFF0000);

    if (frame->vector == 14) {
        uint64_t cr2;

        __asm__ volatile (
            "mov %%cr2, %0"
            : "=r"(cr2)
        );

        printf("CR2: ", 0xFF0000);
        print_hex64(cr2, 0xFF0000);
        printf("\n", 0xFF0000);

        decode_page_fault(frame->error);
    }

    printf("\nRegister Dump:\n", 0xFF0000);

    printf("RAX: ", 0xFF0000);
    print_hex64(frame->rax, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RBX: ", 0xFF0000);
    print_hex64(frame->rbx, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RCX: ", 0xFF0000);
    print_hex64(frame->rcx, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RDX: ", 0xFF0000);
    print_hex64(frame->rdx, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RSI: ", 0xFF0000);
    print_hex64(frame->rsi, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RDI: ", 0xFF0000);
    print_hex64(frame->rdi, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("RBP: ", 0xFF0000);
    print_hex64(frame->rbp, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R8: ", 0xFF0000);
    print_hex64(frame->r8, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R9: ", 0xFF0000);
    print_hex64(frame->r9, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R10: ", 0xFF0000);
    print_hex64(frame->r10, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R11: ", 0xFF0000);
    print_hex64(frame->r11, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R12: ", 0xFF0000);
    print_hex64(frame->r12, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R13: ", 0xFF0000);
    print_hex64(frame->r13, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R14: ", 0xFF0000);
    print_hex64(frame->r14, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("R15: ", 0xFF0000);
    print_hex64(frame->r15, 0xFFFFFF);
    printf("\n", 0xFFFFFF);

    printf("\nSystem halted.\n", 0xFF0000);

    for (;;) {
        __asm__ volatile("cli");
        __asm__ volatile("hlt");
    }
}