# Device Drivers Documentation

## Overview

Device drivers in Entertiment OS provide abstraction layer for hardware access.

## Drivers

### 1. Keyboard Driver

- **Port**: 0x60 (data), 0x64 (status)
- **IRQ**: 1
- **Features**: 
  - Scan code to ASCII conversion
  - Keyboard buffer (circular)
  - Character retrieval

#### Functions
```c
void keyboard_init(void);
char keyboard_getchar(void);
```

### 2. PIT (Programmable Interval Timer)

- **Ports**: 0x40-0x42 (channels), 0x43 (command)
- **IRQ**: 0
- **Features**:
  - Configurable frequency
  - Tick counter
  - Interrupt generation

#### Functions
```c
void timer_init(uint32_t frequency);
uint64_t timer_get_ticks(void);
```

### 3. Serial Port

- **Port**: 0x3F8 (COM1 default)
- **Speed**: 115200 baud
- **Features**:
  - 8-bit data, 1 stop bit
  - FIFO buffering
  - Character and string output

#### Functions
```c
void serial_init(uint16_t port);
void serial_putchar(char c);
void serial_write_string(const char *str);
```

## I/O Port Operations

```c
uint8_t inb(uint16_t port);      /* Read byte */
void outb(uint16_t port, uint8_t value);  /* Write byte */
uint16_t inw(uint16_t port);     /* Read word */
void outw(uint16_t port, uint16_t value); /* Write word */
```

## Interrupt Numbers

- **IRQ 0**: Timer (INT 32)
- **IRQ 1**: Keyboard (INT 33)
- **IRQ 2-15**: Other hardware

## Future Drivers

- [ ] Disk (ATA/SATA)
- [ ] Network (Ethernet)
- [ ] Graphics (VGA/VESA)
- [ ] USB
- [ ] Audio
