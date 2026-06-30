# Vale

Vale is a 64-bit operating system available for the x86_64 architecture.  
At its current state, Vale is extremely limited, but has been tested on real hardware.

I plan to make Vale Unix-like and potentially POSIX-complaint.

I *do not* plan to ever make Vale UNIX certified, nor do I plan to make Vale a part of the GNU Project for now.  
Vale is an independant operating system with no current intentions of replacing existing systems such as GNU/Linux.

Now I am following my own custom model, a hybrid of a Microkernel and a Monolithic-kernel. It reduces the overhead and reduency of microkernel designs with the IPC,
but also less complex and tied together than a Monolithic-kernel, allowing theoretically better stability.

## License

Copyright (C) 2026 Luna Dalenuit and contributers.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

## Build

Required build tools:

* Any GNU/Linux system.
* Python 3.14 `python3`
* GNU C Compiler `gcc`
* Netwide Assembler `nasm`
* GNU Linker `ld`
* Xorriso `xorriso`
* Limine 12 `limine`

Optionally:

* QEMU `qemu-system-x86_64` if you want to emulate Vale.

In the project root, simply run `make`; an ISO file will be generated in ./build.
Alternatively, type `make run` to emulate Vale using QEMU.

## Features

Currently, Vale has:

* BIOS & UEFI Support
* GDT & IDT
* PIC Interrupts
* Stable Heap allocator
* Basic FrameBuffer

Limited to:

* x86_64 Only
* No APIC
* Barely usuable (for now)

Does not have:

* Multi-architectural support
* Filesystems
* Process Switching
* System Calls
* Everything else

## Third-Party Assets

### Limine

Limine is used as the built-in bootloader for Vale.

Copyright (C) 2019-2026 Mintsuki and contributors.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

As of April 7th, 2026, Limine has returned to GitHub.  
Please use this link to view Limine:  
<https://github.com/limine-bootloader/limine>

### Limine Protocol

Limine Protocol is used for the internal systems and ease of use when developing Vale.
It provides an easy to use starting point with BIOS and UEFI support, basic implementations set up.

Copyright (C) 2022-2026 Mintsuki and contributors.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

As of April 7th, 2026, Limine has returned to GitHub.
Please use this link to view the Limine Protocol:  
<https://github.com/Limine-Bootloader/limine-protocol>
