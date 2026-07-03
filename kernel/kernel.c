/* Entertiment OS Kernel */

#include "types.h"
#include "console.h"
#include "memory.h"
#include "interrupts.h"
#include "paging.h"

/* VGA constants */
#define VGA_MEMORY 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

/* Global variables */
static uint16_t *vga_buffer = (uint16_t *) VGA_MEMORY;
static size_t vga_row = 0;
static size_t vga_col = 0;
static uint8_t vga_color = 0x0F; /* White on black */

/* Kernel entry point */
void kernel_main(void)
{
    /* Clear screen */
    terminal_clear();
    
    /* Display welcome message */
    terminal_write_string("=================================\n");
    terminal_write_string("  Entertiment OS Kernel v0.1.0  \n");
    terminal_write_string("=================================\n\n");
    
    /* Initialize subsystems */
    terminal_write_string("[KERNEL] Initializing subsystems...\n\n");
    
    terminal_write_string("[KERNEL] Step 1: Memory Management\n");
    memory_init();
    terminal_write_string("\n");
    
    terminal_write_string("[KERNEL] Step 2: Paging and Virtual Memory\n");
    paging_init();
    terminal_write_string("\n");
    
    terminal_write_string("[KERNEL] Step 3: Interrupts and Exceptions\n");
    interrupts_init();
    terminal_write_string("\n");
    
    terminal_write_string("[KERNEL] System initialization complete!\n");
    terminal_write_string("[KERNEL] Ready for process management.\n\n");
    
    /* Display memory statistics */
    memory_stats();
    
    terminal_write_string("> ");
    
    /* Main kernel loop */
    while (1) {
        asm("hlt");
    }
}

/* Terminal functions */
void terminal_clear(void)
{
    for (size_t i = 0; i < VGA_ROWS * VGA_COLS; i++) {
        vga_buffer[i] = (uint16_t) ' ' | (uint16_t) vga_color << 8;
    }
    vga_row = 0;
    vga_col = 0;
}

void terminal_putchar(char c)
{
    if (c == '\n') {
        vga_col = 0;
        if (++vga_row == VGA_ROWS) {
            vga_row = 0;
        }
        return;
    }
    
    if (c == '\r') {
        vga_col = 0;
        return;
    }
    
    vga_buffer[vga_row * VGA_COLS + vga_col] = 
        (uint16_t) c | (uint16_t) vga_color << 8;
    
    if (++vga_col == VGA_COLS) {
        vga_col = 0;
        if (++vga_row == VGA_ROWS) {
            vga_row = 0;
        }
    }
}

void terminal_write_string(const char *str)
{
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}
