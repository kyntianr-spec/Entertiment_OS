/* Interrupt handling functions */

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "types.h"

/* Interrupt initialization */
void interrupts_init(void);

/* Interrupt handler type */
typedef void (*interrupt_handler_t)(uint32_t int_num);

/* Register interrupt handler */
void register_interrupt(uint8_t num, interrupt_handler_t handler);

#endif /* __INTERRUPTS_H__ */
