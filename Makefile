CC = gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -m32 -I. -fno-pie
LDFLAGS = -T linker.ld -nostdlib -fno-pie -m32 -z max-page-size=0x1000

SRC = src/kernel/main.c src/kernel/syscalls.c src/kernel/vga.c src/kernel/fs.c \
    src/kernel/interrupts.c src/kernel/process.c src/kernel/elf.c src/kernel/string.c \
	src/kernel/linux_binary.c src/kernel/pie_loader.c
OBJ = $(SRC:.c=.o)
BIN = kernel.bin
ISO = os.iso
ASM = src/kernel/interrupt_handlers.asm
ASMOBJ = $(ASM:.asm=.o)

all: $(BIN) $(ISO)

init:
	$(MAKE) -C apps/init

%.o: %.asm
	nasm -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ) $(ASMOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(ASMOBJ)

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