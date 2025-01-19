CC = gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -mcmodel=kernel \
         -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-pie -fno-pic \
         -m64 -mno-80387 -mno-fp-ret-in-387 -fno-stack-protector

LDFLAGS = -T linker.ld -nostdlib -n -z max-page-size=0x1000 \
          -static -no-pie

SRC = src/kernel/main.c src/kernel/syscalls.c src/kernel/vga.c src/kernel/fs.c \
    src/kernel/interrupts.c src/kernel/process.c src/kernel/elf.c src/kernel/string.c \
	src/kernel/linux_binary.c src/kernel/pie_loader.c src/kernel/paging.c
OBJ = $(SRC:.c=.o)
BIN = kernel.bin
ISO = os.iso
ASM = src/kernel/interrupt_handlers.asm src/kernel/long_mode.asm
ASMOBJ = $(ASM:.asm=.o)

all: $(BIN) $(ISO)

init:
	$(MAKE) -C apps/init

debug: $(ISO)
	./run.sh

%.o: %.asm
	nasm -f elf64 -F dwarf -w-other $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ) $(ASMOBJ)
	$(CC) -o $@ $(OBJ) $(ASMOBJ) $(CFLAGS) $(LDFLAGS)

$(ISO): $(BIN) init
	mkdir -p iso/boot/grub
	mkdir -p iso/init
	cp $(BIN) iso/boot/kernel.bin
	cp grub/grub.cfg iso/boot/grub/
	cp apps/init/init iso/init/
	grub-mkrescue -o $@ iso

clean:
	rm -f $(OBJ) $(BIN) $(ISO) $(ASMOBJ)
	$(MAKE) -C apps/init clean
	rm -rf iso