// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    THREAD_STATE_READY,
    THREAD_STATE_RUNNING,
    THREAD_STATE_DEAD,
} thread_state_t;

typedef struct thread {
    void *rsp;
    uint64_t id;
    thread_state_t state;
    void *stack_bottom;
    struct thread *next;
} thread_t;

extern void context_switch(void **old_rsp, void *new_rsp);

void init_scheduler(void);
thread_t *kthread_create(void (*entry_point)(void));
void schedule(void);
void kthread_exit(void);

#endif