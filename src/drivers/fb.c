// SPDX-License-Identifier: GPL-3.0-or-later

#include "fb.h"
#include <stdarg.h>

#define FONT_WIDTH  8
#define FONT_HEIGHT 16

extern uint8_t _binary_font_bin_start[];

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

static struct limine_framebuffer *fb;

static size_t cursor_x = 0;
static size_t cursor_y = 0;

static size_t cols;
static size_t rows;

static uint32_t back;

void init_fb(void) {
    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1) {
        for (;;) __asm__("hlt");
    }

    fb = framebuffer_request.response->framebuffers[0];

    cols = fb->width / FONT_WIDTH;
    rows = fb->height / FONT_HEIGHT;
}

void clear(uint32_t color) {
    back = color;

    uint32_t *dst = (uint32_t *)fb->address;

    for (size_t i = 0; i < (fb->pitch / 4) * fb->height; i++) {
        dst[i] = color;
    }

    cursor_x = 0;
    cursor_y = 0;
}

void set_cursor(size_t x, size_t y) {
    if (x < cols) cursor_x = x;
    if (y < rows) cursor_y = y;
}

static void scroll(uint32_t bg) {
    uint8_t *base = (uint8_t *)fb->address;
    size_t line_size = fb->pitch * FONT_HEIGHT;

    for (size_t y = 1; y < rows; y++) {
        uint8_t *src = base + y * line_size;
        uint8_t *dst = base + (y - 1) * line_size;

        for (size_t i = 0; i < line_size; i++) {
            dst[i] = src[i];
        }
    }

    uint32_t *last = (uint32_t *)(base + (rows - 1) * line_size);
    for (size_t i = 0; i < (fb->pitch / 4) * FONT_HEIGHT; i++) {
        last[i] = bg;
    }
}

static void draw_char(char c, size_t cx, size_t cy, uint32_t fg, uint32_t bg) {
    uint8_t *glyph = &_binary_font_bin_start[(unsigned char)c * 16];

    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
            int bit = (glyph[row] >> (7 - col)) & 1;
            uint32_t color = bit ? fg : bg;

            uint32_t *pixel = (uint32_t *)((uint8_t *)fb->address +
                (cy * FONT_HEIGHT + row) * fb->pitch +
                (cx * FONT_WIDTH + col) * (fb->bpp / 8));

            *pixel = color;
        }
    }
}

void put_char(char c, uint32_t color) {
    uint32_t bg = back;

    switch (c) {
        case '\n':
            cursor_x = 0;
            cursor_y++;
            break;

        case '\r':
            cursor_x = 0;
            break;

        case '\t':
            cursor_x = (cursor_x + 4) & ~(4 - 1);
            break;

        case '\b':
            if (cursor_x > 0) cursor_x--;
            break;

        default:
            draw_char(c, cursor_x, cursor_y, color, bg);
            cursor_x++;
            break;
    }

    if (cursor_x >= cols) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= rows) {
        scroll(bg);
        cursor_y = rows - 1;
    }
}

void puts(const char *str, uint32_t color) {
    while (*str) {
        put_char(*str++, color);
    }
}

void print_hex64(uint64_t value, uint32_t color) {
    char buf[17];

    for (int i = 15; i >= 0; i--) {
        uint8_t digit = value & 0xF;

        buf[i] = digit < 10
            ? '0' + digit
            : 'A' + (digit - 10);

        value >>= 4;
    }

    buf[16] = '\0';

    puts("0x", color);
    puts(buf, color);
}

static void print_num(int val, int base, uint32_t color) {
    char buf[32];
    int i = 0;
    int neg = 0;

    if (val == 0) {
        put_char('0', color);
        return;
    }

    if (base == 10 && val < 0) {
        neg = 1;
        val = -val;
    }

    while (val) {
        int d = val % base;
        buf[i++] = d < 10 ? '0' + d : 'A' + d - 10;
        val /= base;
    }

    if (neg) buf[i++] = '-';

    while (i--) {
        put_char(buf[i], color);
    }
}

void printf(const char *fmt, uint32_t color, ...) {
    va_list args;
    va_start(args, color);

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            put_char(*fmt, color);
            continue;
        }

        fmt++;

        switch (*fmt) {
            case 's':
                puts(va_arg(args, char *), color);
                break;

            case 'c':
                put_char((char)va_arg(args, int), color);
                break;

            case 'd':
                print_num(va_arg(args, int), 10, color);
                break;

            case 'x':
                print_num(va_arg(args, int), 16, color);
                break;

            case '%':
                put_char('%', color);
                break;
        }
    }

    va_end(args);
}