#include "heap.h"
#include "vmm.h"
#include "pmm.h"

#define HEAP_START 0xFFFFFFFFC0000000ULL
#define HEAP_INITIAL_SIZE (16 * 1024 * 1024ULL)

#define HEAP_PAGE_FLAGS (PTE_PRESENT | PTE_WRITE)

static uint64_t heap_base   = HEAP_START;
static uint64_t heap_top    = HEAP_START;
static uint64_t heap_end    = HEAP_START;

static inline uint64_t align_up(uint64_t x, uint64_t align) {
    return (x + align - 1) & ~(align - 1);
}

static void heap_expand(uint64_t bytes) {
    uint64_t pages = align_up(bytes, PAGE_SIZE) / PAGE_SIZE;

    for (uint64_t i = 0; i < pages; i++) {
        uint64_t virt = heap_end;
        uint64_t phys = (uint64_t)pmm_alloc();

        if (!phys) {
            for (;;) __asm__ volatile("hlt");
        }

        vmm_map_page(virt, phys, HEAP_PAGE_FLAGS);

        heap_end += PAGE_SIZE;
    }
}

void init_heap(void) {
    heap_expand(HEAP_INITIAL_SIZE);
}

void *kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = align_up(size, 16);

    if ((heap_top + size) > heap_end) {
        heap_expand(size);
    }

    void *ptr = (void *)heap_top;
    heap_top += size;

    return ptr;
}