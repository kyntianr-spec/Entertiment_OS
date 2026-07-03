/* Memory Management Implementation */

#include "memory.h"
#include "console.h"
#include "types.h"

/* Physical memory constants */
#define PAGE_SIZE 4096
#define KERNEL_BASE 0x100000

/* Multiboot info structure (provided by bootloader) */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
};

/* Memory map entry */
struct mmap_entry {
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
};

/* Global memory info */
static struct {
    uint64_t total_memory;
    uint64_t available_memory;
    uint32_t num_pages;
    uint8_t *bitmap;
} mem_info = {0};

/* Simple heap allocator */
#define HEAP_SIZE (10 * 1024 * 1024)  /* 10 MB */
static uint8_t heap[HEAP_SIZE] = {0};
static size_t heap_ptr = 0;

/* Initialize memory management */
void memory_init(void)
{
    terminal_write_string("[MEMORY] Initializing memory management...\n");
    
    /* Get total memory from BIOS (simplified) */
    /* In real implementation, would parse Multiboot memory map */
    mem_info.total_memory = 256 * 1024 * 1024;  /* Assume 256 MB */
    mem_info.available_memory = mem_info.total_memory - KERNEL_BASE;
    mem_info.num_pages = mem_info.total_memory / PAGE_SIZE;
    
    terminal_write_string("[MEMORY] Total memory: ");
    print_memory_size(mem_info.total_memory);
    terminal_write_string("\n");
    
    terminal_write_string("[MEMORY] Available memory: ");
    print_memory_size(mem_info.available_memory);
    terminal_write_string("\n");
    
    terminal_write_string("[MEMORY] Total pages: ");
    print_number(mem_info.num_pages);
    terminal_write_string("\n");
    
    /* Initialize heap */
    heap_ptr = 0;
    terminal_write_string("[MEMORY] Heap initialized (");
    print_memory_size(HEAP_SIZE);
    terminal_write_string(")\n");
}

/* Simple malloc - linear allocator for now */
void *malloc(size_t size)
{
    if (heap_ptr + size > HEAP_SIZE) {
        terminal_write_string("[MEMORY] Heap overflow!\n");
        return NULL;
    }
    
    void *ptr = (void *)(heap + heap_ptr);
    heap_ptr += size;
    
    /* Align to 16 bytes */
    if (heap_ptr % 16 != 0) {
        heap_ptr += 16 - (heap_ptr % 16);
    }
    
    return ptr;
}

/* Free memory (simplified - doesn't actually free) */
void free(void *ptr)
{
    /* In a real implementation, would use a proper allocator */
    (void)ptr;
}

/* Memory copy */
void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

/* Memory set */
void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
    
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
    
    return s;
}

/* Memory comparison */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    
    return 0;
}

/* Get memory statistics */
void memory_stats(void)
{
    terminal_write_string("\n=== Memory Statistics ===\n");
    terminal_write_string("Total Memory: ");
    print_memory_size(mem_info.total_memory);
    terminal_write_string("\n");
    
    terminal_write_string("Available: ");
    print_memory_size(mem_info.available_memory);
    terminal_write_string("\n");
    
    terminal_write_string("Heap Used: ");
    print_memory_size(heap_ptr);
    terminal_write_string(" / ");
    print_memory_size(HEAP_SIZE);
    terminal_write_string("\n");
    
    terminal_write_string("Heap Free: ");
    print_memory_size(HEAP_SIZE - heap_ptr);
    terminal_write_string("\n");
    
    terminal_write_string("========================\n\n");
}

/* Helper: Print number */
void print_number(uint64_t num)
{
    if (num == 0) {
        terminal_putchar('0');
        return;
    }
    
    char buffer[20];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        terminal_putchar(buffer[--i]);
    }
}

/* Helper: Print memory size (e.g., 2 MB) */
void print_memory_size(uint64_t bytes)
{
    if (bytes >= 1024 * 1024 * 1024) {
        print_number(bytes / (1024 * 1024 * 1024));
        terminal_write_string(" GB");
    } else if (bytes >= 1024 * 1024) {
        print_number(bytes / (1024 * 1024));
        terminal_write_string(" MB");
    } else if (bytes >= 1024) {
        print_number(bytes / 1024);
        terminal_write_string(" KB");
    } else {
        print_number(bytes);
        terminal_write_string(" B");
    }
}

/* Get heap usage percentage */
uint32_t get_heap_usage_percent(void)
{
    return (heap_ptr * 100) / HEAP_SIZE;
}
