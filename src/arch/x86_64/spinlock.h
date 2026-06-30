// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

typedef struct {
    bool locked;
} spinlock_t;

#define SPINLOCK_INIT { false }

static inline void spin_lock(spinlock_t *lock) {
    __asm__ volatile("cli");
    
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        __asm__ volatile("pause");
    }
}

static inline void spin_unlock(spinlock_t *lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
    
    __asm__ volatile("sti");
}

#endif