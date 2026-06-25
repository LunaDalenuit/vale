; SPDX-License-Identifier: GPL-3.0-or-later

BITS 64

global isr0
global isr8
global isr13
global isr14

global irq0
global irq1

extern exception_handler
extern irq_handler

section .text

%macro PUSH_REGS 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
%endmacro

%macro POP_REGS 0
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro ISR_NOERR 1
isr%1:
    push 0
    push %1
    PUSH_REGS

    mov rdi, rsp
    call exception_handler

    POP_REGS
    add rsp, 16
    iretq
%endmacro

%macro ISR_ERR 1
isr%1:
    push %1

    PUSH_REGS

    mov rdi, rsp
    call exception_handler

    POP_REGS
    add rsp, 16
    iretq
%endmacro

ISR_NOERR 0

ISR_ERR 8
ISR_ERR 13
ISR_ERR 14

irq0:
    push 0
    push 32

    PUSH_REGS

    mov rdi, rsp
    call irq_handler

    POP_REGS
    add rsp, 16
    iretq

irq1:
    push 0
    push 33

    PUSH_REGS

    mov rdi, rsp
    call irq_handler

    POP_REGS
    add rsp, 16
    iretq