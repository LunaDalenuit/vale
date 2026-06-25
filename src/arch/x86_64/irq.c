// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
#include "exceptions.h"
#include "../../drivers/fb.h"
#include "pic.h"
#include "keyboard.h"

static volatile uint64_t ticks = 0;

void irq_handler(interrupt_frame_t *frame) {
    if (frame->vector == 32) { // Timer
        ticks++;
        // printf("IRQ0 FIRED: Tick %d\n", 0x00FF00, ticks);

        pic_send_eoi(0); 
        return;
    }

    if (frame->vector == 33) { // Keyboard
        keyboard_handler_main();
        pic_send_eoi(1);
        return;
    }
    
    printf("UNHANDLED IRQ VECTOR: %d\n", 0xFF0000, frame->vector);
}