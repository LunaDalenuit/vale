// SPDX-License-Identifier: GPL-3.0-or-later

#include "thread.h"
#include "../../drivers/memory/heap.h"
#include "../../drivers/fb.h"

static thread_t *running_thread = NULL;
static thread_t *thread_queue_head = NULL;
static uint64_t next_thread_id = 1;

// Dummy TCB to represent boot sequence execution context
static thread_t main_kernel_thread;

void init_scheduler(void) {
    main_kernel_thread.id = 0;
    main_kernel_thread.state = THREAD_STATE_RUNNING;
    main_kernel_thread.stack_bottom = NULL; 
    main_kernel_thread.rsp = NULL;
    main_kernel_thread.next = NULL;

    running_thread = &main_kernel_thread;
    thread_queue_head = &main_kernel_thread;
    
    printf("SCHEDULER: Initialized round-robin execution queue.\n", 0x00FFFF);
}

thread_t *kthread_create(void (*entry_point)(void)) {
    thread_t *thread = (thread_t *)kmalloc(sizeof(thread_t));

    if (!thread) return NULL;

    size_t stack_size = 16384;
    void *stack_raw = kmalloc(stack_size);
    if (!stack_raw) {
        kfree(thread);
        return NULL;
    }

    thread->id = next_thread_id++;
    
    thread->state = THREAD_STATE_READY; 
    
    thread->stack_bottom = stack_raw;
    thread->next = NULL;

    uint64_t *stack_top = (uint64_t *)((uintptr_t)stack_raw + stack_size);

    stack_top--;
    *stack_top = (uint64_t)kthread_exit;

    stack_top--;
    *stack_top = (uint64_t)entry_point;

    for (int i = 0; i < 6; i++) {
        stack_top--;
        *stack_top = 0;
    }

    thread->rsp = (void *)stack_top;

    thread_t *curr = thread_queue_head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = thread;

    return thread;
}

void kthread_exit(void) {
    __asm__ volatile("cli");
    running_thread->state = THREAD_STATE_DEAD;

    schedule();

    for (;;) __asm__ volatile("hlt");
}

void schedule(void) {
    if (!thread_queue_head) return;
    thread_t *old_thread = running_thread;
    thread_t *new_thread = old_thread->next;

    while (1) {
        if (!new_thread) {
            new_thread = thread_queue_head;
        }

        if (new_thread->state == THREAD_STATE_READY || new_thread->state == THREAD_STATE_RUNNING) {
            break;
        }

        if (new_thread->state == THREAD_STATE_DEAD) {
            new_thread = new_thread->next;
        }
    }

    if (new_thread == old_thread) {
        return;
    }

    if (old_thread->state == THREAD_STATE_RUNNING) {
        old_thread->state = THREAD_STATE_READY;
    }

    new_thread->state = THREAD_STATE_RUNNING;
    running_thread = new_thread;

    context_switch(&old_thread->rsp, new_thread->rsp);
}