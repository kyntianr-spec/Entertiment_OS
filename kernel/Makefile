CC = gcc
AS = nasm
LD = ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -I./include
ASFLAGS = -f elf64
LDFLAGS = -T linker.ld -nostdlib

KERNEL_SRC = kernel.c memory.c interrupts.c paging.c drivers.c
KERNEL_ASM = entry.asm interrupts.asm
KERNEL_OBJ = $(KERNEL_SRC:.c=.o) $(KERNEL_ASM:.asm=.o)

KERNEL_BIN = kernel.bin

all: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL_OBJ)
	@echo "[KERNEL] Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "[KERNEL] Kernel compiled: $@"

%.o: %.c
	@echo "[KERNEL] Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo "[KERNEL] Assembling $<..."
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	@echo "[KERNEL] Cleaning..."
	rm -f $(KERNEL_OBJ) $(KERNEL_BIN)
	@echo "[KERNEL] Cleaned!"

.PHONY: all clean
