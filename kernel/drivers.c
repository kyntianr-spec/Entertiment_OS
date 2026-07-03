/* Device Drivers Implementation */

#include "drivers.h"
#include "interrupts.h"
#include "console.h"
#include "types.h"

/* ==================== KEYBOARD DRIVER ==================== */

/* Keyboard constants */
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64

/* Keyboard buffer */
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static size_t keyboard_buffer_head = 0;
static size_t keyboard_buffer_tail = 0;

/* Keyboard scan codes to ASCII */
static const char keyboard_scancode_table[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, '*', 0, ' '
};

/* Initialize keyboard */
void keyboard_init(void)
{
    terminal_write_string("[KEYBOARD] Initializing keyboard...\n");
    register_interrupt(33, keyboard_handler);
    terminal_write_string("[KEYBOARD] Keyboard initialized (IRQ 1)\n");
}

/* Keyboard interrupt handler */
void keyboard_handler(uint32_t int_num)
{
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode < sizeof(keyboard_scancode_table)) {
        char c = keyboard_scancode_table[scancode];
        if (c != 0) {
            keyboard_buffer[keyboard_buffer_head] = c;
            keyboard_buffer_head = (keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        }
    }
    
    /* Send EOI to PIC */
    outb(0x20, 0x20);
}

/* Get character from keyboard buffer */
char keyboard_getchar(void)
{
    if (keyboard_buffer_head == keyboard_buffer_tail) {
        return 0;  /* Buffer empty */
    }
    
    char c = keyboard_buffer[keyboard_buffer_tail];
    keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

/* ==================== TIMER DRIVER ==================== */

/* Timer constants */
#define PIT_CHANNEL0        0x40
#define PIT_CHANNEL1        0x41
#define PIT_CHANNEL2        0x42
#define PIT_COMMAND         0x43
#define PIT_FREQUENCY       1193182

/* Timer state */
static uint64_t timer_ticks = 0;

/* Initialize timer */
void timer_init(uint32_t frequency)
{
    terminal_write_string("[TIMER] Initializing timer (");
    print_number(frequency);
    terminal_write_string(" Hz)...\n");
    
    /* Calculate divisor */
    uint16_t divisor = PIT_FREQUENCY / frequency;
    
    /* Set command byte */
    outb(PIT_COMMAND, 0x36);  /* Channel 0, both bytes, mode 3 */
    
    /* Send divisor */
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
    
    register_interrupt(32, timer_handler);
    terminal_write_string("[TIMER] Timer initialized (IRQ 0)\n");
}

/* Timer interrupt handler */
void timer_handler(uint32_t int_num)
{
    timer_ticks++;
    
    /* Send EOI to PIC */
    outb(0x20, 0x20);
}

/* Get timer ticks */
uint64_t timer_get_ticks(void)
{
    return timer_ticks;
}

/* ==================== SERIAL PORT DRIVER ==================== */

/* Serial constants */
#define SERIAL_PORT_A       0x3F8
#define SERIAL_DATA         0
#define SERIAL_DLAB         0x80

/* Initialize serial port */
void serial_init(uint16_t port)
{
    terminal_write_string("[SERIAL] Initializing serial port 0x");
    print_hex(port);
    terminal_write_string("...\n");
    
    /* Set DLAB */
    outb(port + 3, SERIAL_DLAB);
    
    /* Set baud rate to 115200 */
    outb(port + 0, 1);
    outb(port + 1, 0);
    
    /* Set line control */
    outb(port + 3, 0x03);  /* 8 bits, 1 stop, no parity */
    
    /* Enable FIFO */
    outb(port + 2, 0xC7);
    
    /* Set modem control */
    outb(port + 4, 0x0B);
    
    terminal_write_string("[SERIAL] Serial port initialized\n");
}

/* Send character to serial port */
void serial_putchar(char c)
{
    uint16_t port = SERIAL_PORT_A;
    
    /* Wait for transmit buffer empty */
    while ((inb(port + 5) & 0x20) == 0);
    
    outb(port, c);
}

/* Send string to serial port */
void serial_write_string(const char *str)
{
    for (size_t i = 0; str[i] != '\0'; i++) {
        serial_putchar(str[i]);
    }
}

/* ==================== I/O PORT HELPERS ==================== */

/* Read byte from I/O port */
uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* Write byte to I/O port */
void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Read word from I/O port */
uint16_t inw(uint16_t port)
{
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* Write word to I/O port */
void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}
