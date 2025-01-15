#!/bin/bash
qemu-system-x86_64 -cdrom os.iso -d int,cpu_reset -no-reboot -no-shutdown -monitor stdio