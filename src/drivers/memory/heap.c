#include "heap.h"
#include "vmm.h"
#include "pmm.h"

#define HEAP_START 0xFFFFFFFFC0000000ULL
#define HEAP_INITIAL_SIZE (16 * 1024 * 1024ULL)
#define HEAP_PAGE_FLAGS (PTE_PRESENT | PTE_WRITE)

static heap_block_t *head = NULL;
static uint64_t heap_end = HEAP_START;

static inline uint64_t align_up(uint64_t x, uint64_t align) {
    return (x + align - 1) & ~(align -  1);
}

static heap_block_t *heap_expand(size_t bytes) {
    uint64_t needed = align_up(bytes, PAGE_SIZE);
    uint64_t pages = needed / PAGE_SIZE;

    uint64_t start_virt = heap_end;

    for (uint64_t i = 0; i < pages; i++) {
        uint64_t phys = (uint64_t)pmm_alloc();
        if (!phys) {
            printf("HEAP: Out of physical memory. Halting...\n", 0xFF0000);
            for (;;) __asm__ volatile("hlt");
        }
        vmm_map_page(heap_end, phys, HEAP_PAGE_FLAGS);
        heap_end += PAGE_SIZE;
    }

    heap_block_t *new_block = (heap_block_t *)start_virt;
    new_block->size = needed - sizeof(heap_block_t);
    new_block->is_free = true;
    new_block->next = NULL;

    return new_block;
}

void init_heap(void) {
    head = heap_expand(HEAP_INITIAL_SIZE);
    printf("HEAP: Succesfully initialized.\n", 0x00FFFF);
}

void *kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = align_up(size, 16);

    heap_block_t *current = head;
    heap_block_t *last = NULL;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            if (current->size >= size + sizeof(heap_block_t) + 16) {
                heap_block_t *new_block = (heap_block_t *)((uintptr_t)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->is_free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }
            current->is_free = false;
            return (void*)((uintptr_t)current + sizeof(heap_block_t));
        }
        last = current;
        current = current->next;
    }

    size_t needed_bytes = size + sizeof(heap_block_t);
    heap_block_t *new_block = heap_expand(needed_bytes);
    
    if (last != NULL) {
        last->next = new_block;
    } else {
        if (head == NULL) head = new_block;
    }

    if (new_block->size >= size + sizeof(heap_block_t) + 16) {
        heap_block_t *split_block = (heap_block_t *)((uintptr_t)new_block + sizeof(heap_block_t) + size);
        split_block->size = new_block->size - size - sizeof(heap_block_t);
        split_block->is_free = true;
        split_block->next = new_block->next;

        new_block->size = size;
        new_block->next = split_block;
    }

    new_block->is_free = false;
    return (void *)((uintptr_t)new_block + sizeof(heap_block_t));
}

void kfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    heap_block_t *block = (heap_block_t *)((uintptr_t)ptr - sizeof(heap_block_t));
    block->is_free = true;

    heap_block_t *current = head;
    while (current != NULL) {
        while (current->is_free && current->next != NULL && current->next->is_free) {
            current->size += sizeof(heap_block_t) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}