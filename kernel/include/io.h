/* I/O Port helpers declarations */

#ifndef __IO_H__
#define __IO_H__

#include "types.h"

/* Read/Write I/O ports */
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

#endif /* __IO_H__ */
