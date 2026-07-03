/* Interrupt Descriptor Table and Exception Handling */

#include "interrupts.h"
#include "console.h"
#include "types.h"

/* IDT entry structure */
struct idt_entry {
    uint16_t offset_low;    /* Offset bits 0-15 */
    uint16_t selector;      /* Selector */
    uint8_t zero;           /* Always zero */
    uint8_t flags;          /* Flags */
    uint16_t offset_high;   /* Offset bits 16-31 */
} __attribute__((packed));

/* IDT register */
struct idt_register {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/* Interrupt frame (pushed by CPU/handlers) */
struct interrupt_frame {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t int_num;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

/* IDT table */
#define IDT_ENTRIES 256
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_register idt_reg;

/* Interrupt handlers table */
static interrupt_handler_t handlers[IDT_ENTRIES] = {NULL};

/* Flags for IDT entry */
#define IDT_PRESENT      0x80
#define IDT_RING0        0x00
#define IDT_RING3        0x60
#define IDT_INTERRUPT    0x0E
#define IDT_TRAP         0x0F

/* Exception names */
static const char *exception_names[] = {
    "Division by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

/* Set IDT entry */
static void set_idt_entry(uint8_t num, uint64_t handler, uint8_t flags)
{
    idt[num].offset_low = (uint16_t)(handler & 0xFFFF);
    idt[num].selector = 0x08;  /* Code segment */
    idt[num].zero = 0;
    idt[num].flags = flags;
    idt[num].offset_high = (uint16_t)((handler >> 16) & 0xFFFF);
}

/* Load IDT */
static void load_idt(void)
{
    idt_reg.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idt_reg.base = (uint64_t)&idt;
    
    asm volatile("lidt %0" : : "m"(idt_reg));
}

/* Exception handlers (stubs - implemented in interrupts.asm) */
extern void exception_0(void);
extern void exception_1(void);
extern void exception_2(void);
extern void exception_3(void);
extern void exception_4(void);
extern void exception_5(void);
extern void exception_6(void);
extern void exception_7(void);
extern void exception_8(void);
extern void exception_9(void);
extern void exception_10(void);
extern void exception_11(void);
extern void exception_12(void);
extern void exception_13(void);
extern void exception_14(void);
extern void exception_15(void);
extern void exception_16(void);
extern void exception_17(void);
extern void exception_18(void);
extern void exception_19(void);
extern void exception_20(void);

/* Default exception handler */
void default_exception_handler(struct interrupt_frame *frame)
{
    uint8_t int_num = frame->int_num & 0xFF;
    
    terminal_write_string("\n=== EXCEPTION ===\n");
    terminal_write_string("Exception: ");
    
    if (int_num < 32) {
        terminal_write_string((char *)exception_names[int_num]);
    } else {
        terminal_write_string("Unknown");
    }
    
    terminal_write_string("\nNumber: ");
    print_hex(int_num);
    
    if (frame->error_code != 0) {
        terminal_write_string("\nError Code: ");
        print_hex(frame->error_code);
    }
    
    terminal_write_string("\nRIP: ");
    print_hex(frame->rip);
    
    terminal_write_string("\nRSP: ");
    print_hex(frame->rsp);
    
    terminal_write_string("\n==================\n");
    
    /* Halt */
    while (1) {
        asm("hlt");
    }
}

/* Generic interrupt handler wrapper */
void interrupt_handler_wrapper(struct interrupt_frame *frame)
{
    uint8_t int_num = frame->int_num & 0xFF;
    
    if (handlers[int_num] != NULL) {
        handlers[int_num](int_num);
    } else {
        default_exception_handler(frame);
    }
}

/* Initialize interrupts */
void interrupts_init(void)
{
    terminal_write_string("[INTERRUPTS] Initializing IDT...\n");
    
    /* Clear IDT */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        set_idt_entry(i, 0, 0);
    }
    
    /* Set exception handlers */
    set_idt_entry(0, (uint64_t)exception_0, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(1, (uint64_t)exception_1, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(2, (uint64_t)exception_2, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(3, (uint64_t)exception_3, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(4, (uint64_t)exception_4, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(5, (uint64_t)exception_5, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(6, (uint64_t)exception_6, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(7, (uint64_t)exception_7, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(8, (uint64_t)exception_8, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(9, (uint64_t)exception_9, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(10, (uint64_t)exception_10, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(11, (uint64_t)exception_11, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(12, (uint64_t)exception_12, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(13, (uint64_t)exception_13, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(14, (uint64_t)exception_14, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(15, (uint64_t)exception_15, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(16, (uint64_t)exception_16, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(17, (uint64_t)exception_17, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(18, (uint64_t)exception_18, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(19, (uint64_t)exception_19, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    set_idt_entry(20, (uint64_t)exception_20, IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
    
    /* Load IDT */
    load_idt();
    
    terminal_write_string("[INTERRUPTS] IDT loaded with ");
    print_number(IDT_ENTRIES);
    terminal_write_string(" entries\n");
    
    /* Enable interrupts */
    asm volatile("sti");
    terminal_write_string("[INTERRUPTS] Interrupts enabled\n");
}

/* Register custom interrupt handler */
void register_interrupt(uint8_t num, interrupt_handler_t handler)
{
    if (num < IDT_ENTRIES) {
        handlers[num] = handler;
        terminal_write_string("[INTERRUPTS] Registered handler for interrupt ");
        print_number(num);
        terminal_write_string("\n");
    }
}

/* Helper: Print hex value */
void print_hex(uint64_t value)
{
    const char *hex_digits = "0123456789ABCDEF";
    char buffer[16];
    int i = 0;
    
    for (int j = 60; j >= 0; j -= 4) {
        uint8_t digit = (value >> j) & 0xF;
        if (digit != 0 || i > 0) {
            buffer[i++] = hex_digits[digit];
        }
    }
    
    if (i == 0) {
        buffer[i++] = '0';
    }
    
    buffer[i] = '\0';
    terminal_write_string("0x");
    terminal_write_string(buffer);
}
