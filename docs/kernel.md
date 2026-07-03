# Kernel Architecture

## Overview

The Entertiment OS kernel is the core of the operating system, providing essential services and managing hardware resources.

## Kernel Components

### 1. Entry Point (entry.asm)
- 64-bit assembly entry point
- Stack initialization
- Calls kernel_main()

### 2. Console/Terminal (kernel.c)
- VGA text mode output
- String printing utilities
- Screen clearing

### 3. Memory Management
- Physical memory tracking
- Virtual memory with paging
- Heap allocation (malloc/free)
- Memory protection

### 4. Interrupt Handling
- IDT (Interrupt Descriptor Table) setup
- Interrupt handlers
- Exception handling

### 5. Process Management
- Process/task creation and termination
- Context switching
- Scheduling
- IPC (Inter-Process Communication)

### 6. Device Drivers
- Console/keyboard
- Disk (ATA/SATA)
- Network (future)

## Memory Layout

```
0x000000 - 0x0FFFFF    : Real Mode IVT, BIOS Data
0x100000 - ...         : Kernel (1MB+)
```

## Build

```bash
make kernel
```

## Future Improvements

- [ ] Full memory management system
- [ ] Interrupt handler implementation
- [ ] Process scheduler
- [ ] Virtual filesystem
- [ ] System calls
