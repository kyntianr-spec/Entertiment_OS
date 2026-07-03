/* Device Drivers */

#ifndef __DRIVERS_H__
#define __DRIVERS_H__

#include "types.h"

/* Keyboard driver */
void keyboard_init(void);
void keyboard_handler(uint32_t int_num);
char keyboard_getchar(void);

/* Timer driver */
void timer_init(uint32_t frequency);
void timer_handler(uint32_t int_num);
uint64_t timer_get_ticks(void);

/* Serial port driver */
void serial_init(uint16_t port);
void serial_putchar(char c);
void serial_write_string(const char *str);

#endif /* __DRIVERS_H__ */
