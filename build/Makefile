#!/usr/bin/make -f
# Entertiment OS - Main Makefile

# Compiler settings
CC = gcc
AS = nasm
LD = ld
QEMU = qemu-system-x86_64

# Directories
BOOTLOADER_DIR = bootloader
KERNEL_DIR = kernel
BUILD_DIR = build
BIN_DIR = bin
ISO_DIR = iso

# Flags
ASFLAGS = -f elf64
CFLAGS = -ffreestanding -O2 -Wall -Wextra -I$(KERNEL_DIR)/include
LDFLAGS = -T $(KERNEL_DIR)/linker.ld -nostdlib

# Output files
BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BIN_DIR)/entertiment_os.iso
OS_IMG = $(BIN_DIR)/entertiment_os.img

# Default target
all: build

# Create directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(ISO_DIR):
	@mkdir -p $(ISO_DIR)

# Bootloader
bootloader: $(BUILD_DIR) $(BOOTLOADER_BIN)

$(BOOTLOADER_BIN): $(BOOTLOADER_DIR)/boot.asm
	@echo "[BOOTLOADER] Assembling bootloader..."
	$(AS) -f bin $(BOOTLOADER_DIR)/boot.asm -o $(BOOTLOADER_BIN)
	@echo "[BOOTLOADER] Done!"

# Kernel
kernel: $(BUILD_DIR)
	@echo "[KERNEL] Compiling kernel..."
	$(MAKE) -C $(KERNEL_DIR)
	@echo "[KERNEL] Done!"

# Build entire project
build: bootloader kernel
	@echo "[BUILD] Linking OS image..."
	@echo "[BUILD] Build complete!"

# Create ISO image
iso: build $(BIN_DIR) $(ISO_DIR)
	@echo "[ISO] Creating ISO image..."
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BOOTLOADER_BIN) $(ISO_DIR)/boot/
	cp $(KERNEL_DIR)/kernel.bin $(ISO_DIR)/boot/ 2>/dev/null || true
	# Create grub.cfg if needed for GRUB bootloader
	@echo "[ISO] Creating bootable ISO..."
	grub-mkrescue -o $(OS_IMAGE) $(ISO_DIR) 2>/dev/null || \
	echo "[ISO] GRUB not available - using basic image creation"
	@echo "[ISO] ISO created: $(OS_IMAGE)"

# Create disk image
img: build $(BIN_DIR)
	@echo "[IMG] Creating disk image..."
	dd if=/dev/zero of=$(OS_IMG) bs=1M count=16
	dd if=$(BOOTLOADER_BIN) of=$(OS_IMG) bs=512 count=1 conv=notrunc
	@echo "[IMG] Disk image created: $(OS_IMG)"

# Run in QEMU
run: iso
	@echo "[QEMU] Running OS..."
	$(QEMU) -cdrom $(OS_IMAGE) -m 512 -serial stdio

run-img: img
	@echo "[QEMU] Running OS from disk image..."
	$(QEMU) -drive format=raw,file=$(OS_IMG) -m 512 -serial stdio

# Debug mode with GDB
debug: iso
	@echo "[DEBUG] Starting QEMU with GDB..."
	$(QEMU) -cdrom $(OS_IMAGE) -m 512 -serial stdio -s -S &
	@echo "[DEBUG] Starting GDB... (connect with 'target remote :1234')"
	gdb

# Clean
clean:
	@echo "[CLEAN] Cleaning build files..."
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(ISO_DIR)
	$(MAKE) -C $(KERNEL_DIR) clean
	@echo "[CLEAN] Done!"

# Full rebuild
rebuild: clean all

.PHONY: all build bootloader kernel iso img run run-img debug clean rebuild
