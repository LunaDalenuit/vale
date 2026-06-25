// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void remap_pic(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);

#endif