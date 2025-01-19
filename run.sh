#!/bin/bash
qemu-system-x86_64 \
    -cdrom os.iso \
    -d int,cpu_reset \
    -debugcon file:logs/vga.log \
    -no-reboot \
    -no-shutdown \
    -monitor stdio
    