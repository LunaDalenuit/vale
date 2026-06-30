// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct heap_block {
    size_t size;
    bool is_free;
    struct heap_block *next;
} heap_block_t;

void init_heap(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif