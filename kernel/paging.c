/* Paging and Virtual Memory Implementation */

#include "paging.h"
#include "memory.h"
#include "console.h"
#include "types.h"

/* Page table structures */
#define PAGE_PRESENT    0x01
#define PAGE_WRITE      0x02
#define PAGE_USER       0x04
#define PAGE_WT         0x08
#define PAGE_CD         0x10
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40
#define PAGE_PAT        0x80

/* PML4 (Page Map Level 4) - Level 1 paging structure */
static uint64_t pml4[512] __attribute__((aligned(4096)));

/* PDPT (Page Directory Pointer Table) - Level 2 */
static uint64_t pdpt[512] __attribute__((aligned(4096)));

/* PD (Page Directory) - Level 3 */
static uint64_t pd[512] __attribute__((aligned(4096)));

/* PT (Page Table) - Level 4 */
static uint64_t pt[512] __attribute__((aligned(4096)));

/* Page allocation bitmap */
#define MAX_PAGES 65536
static uint8_t page_bitmap[MAX_PAGES / 8];

/* Initialize paging structures */
void paging_init(void)
{
    terminal_write_string("[PAGING] Initializing paging...\n");
    
    /* Clear all paging structures */
    for (int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pdpt[i] = 0;
        pd[i] = 0;
        pt[i] = 0;
    }
    
    /* Clear page bitmap */
    for (int i = 0; i < MAX_PAGES / 8; i++) {
        page_bitmap[i] = 0;
    }
    
    /* Map first 2MB (kernel space) using 2MB pages */
    /* PML4[0] -> PDPT */
    pml4[0] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITE;
    
    /* PDPT[0] -> PD */
    pdpt[0] = (uint64_t)pd | PAGE_PRESENT | PAGE_WRITE;
    
    /* PD[0] -> 2MB page (kernel) */
    pd[0] = 0x000000 | PAGE_PRESENT | PAGE_WRITE | 0x80;  /* 2MB page flag */
    
    /* PD[1] -> 2MB page (kernel continue) */
    pd[1] = 0x200000 | PAGE_PRESENT | PAGE_WRITE | 0x80;
    
    /* Map VGA memory (0xB8000) at higher half */
    /* PML4[256] for higher half mapping */
    pml4[256] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITE;
    
    terminal_write_string("[PAGING] Paging structures initialized\n");
    terminal_write_string("[PAGING] PML4: ");
    print_hex((uint64_t)pml4);
    terminal_write_string("\n");
    
    /* Enable paging */
    enable_paging();
}

/* Enable paging */
void enable_paging(void)
{
    terminal_write_string("[PAGING] Enabling paging...\n");
    
    /* Set CR3 to PML4 */
    asm volatile("mov %0, %%cr3" : : "r"((uint64_t)pml4));
    
    /* Set CR4.PAE (Physical Address Extension) */
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x20;  /* PAE bit */
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
    
    /* Set CR0.PG (Paging Enable) */
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  /* PG bit */
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
    
    terminal_write_string("[PAGING] Paging enabled\n");
}

/* Allocate a page */
uint64_t page_alloc(void)
{
    /* Find free page in bitmap */
    for (int i = 0; i < MAX_PAGES / 8; i++) {
        if (page_bitmap[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                if ((page_bitmap[i] & (1 << j)) == 0) {
                    /* Found free page */
                    page_bitmap[i] |= (1 << j);
                    return (uint64_t)((i * 8 + j) * 4096);
                }
            }
        }
    }
    
    return 0;  /* No free pages */
}

/* Free a page */
void page_free(uint64_t page)
{
    uint64_t page_num = page / 4096;
    uint8_t byte_idx = page_num / 8;
    uint8_t bit_idx = page_num % 8;
    
    if (byte_idx < MAX_PAGES / 8) {
        page_bitmap[byte_idx] &= ~(1 << bit_idx);
    }
}

/* Map a virtual address to physical address */
void page_map(uint64_t virt, uint64_t phys, uint64_t flags)
{
    /* Extract page table indices */
    uint16_t pml4_idx = (virt >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virt >> 30) & 0x1FF;
    uint16_t pd_idx = (virt >> 21) & 0x1FF;
    uint16_t pt_idx = (virt >> 12) & 0x1FF;
    
    /* Check if page tables exist, create if necessary */
    if ((pml4[pml4_idx] & PAGE_PRESENT) == 0) {
        uint64_t new_pdpt = page_alloc();
        if (new_pdpt == 0) return;
        pml4[pml4_idx] = new_pdpt | PAGE_PRESENT | PAGE_WRITE;
    }
    
    uint64_t *pdpt_ptr = (uint64_t *)(pml4[pml4_idx] & ~0xFFF);
    
    if ((pdpt_ptr[pdpt_idx] & PAGE_PRESENT) == 0) {
        uint64_t new_pd = page_alloc();
        if (new_pd == 0) return;
        pdpt_ptr[pdpt_idx] = new_pd | PAGE_PRESENT | PAGE_WRITE;
    }
    
    uint64_t *pd_ptr = (uint64_t *)(pdpt_ptr[pdpt_idx] & ~0xFFF);
    
    if ((pd_ptr[pd_idx] & PAGE_PRESENT) == 0) {
        uint64_t new_pt = page_alloc();
        if (new_pt == 0) return;
        pd_ptr[pd_idx] = new_pt | PAGE_PRESENT | PAGE_WRITE;
    }
    
    uint64_t *pt_ptr = (uint64_t *)(pd_ptr[pd_idx] & ~0xFFF);
    
    /* Set page table entry */
    pt_ptr[pt_idx] = phys | flags | PAGE_PRESENT;
    
    /* Invalidate TLB entry */
    asm volatile("invlpg %0" : : "m"(*(char *)virt));
}

/* Unmap a virtual address */
void page_unmap(uint64_t virt)
{
    uint16_t pml4_idx = (virt >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virt >> 30) & 0x1FF;
    uint16_t pd_idx = (virt >> 21) & 0x1FF;
    uint16_t pt_idx = (virt >> 12) & 0x1FF;
    
    if ((pml4[pml4_idx] & PAGE_PRESENT) == 0) return;
    
    uint64_t *pdpt_ptr = (uint64_t *)(pml4[pml4_idx] & ~0xFFF);
    if ((pdpt_ptr[pdpt_idx] & PAGE_PRESENT) == 0) return;
    
    uint64_t *pd_ptr = (uint64_t *)(pdpt_ptr[pdpt_idx] & ~0xFFF);
    if ((pd_ptr[pd_idx] & PAGE_PRESENT) == 0) return;
    
    uint64_t *pt_ptr = (uint64_t *)(pd_ptr[pd_idx] & ~0xFFF);
    pt_ptr[pt_idx] = 0;
    
    /* Invalidate TLB entry */
    asm volatile("invlpg %0" : : "m"(*(char *)virt));
}

/* Get physical address for virtual address */
uint64_t page_get_physical(uint64_t virt)
{
    uint16_t pml4_idx = (virt >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virt >> 30) & 0x1FF;
    uint16_t pd_idx = (virt >> 21) & 0x1FF;
    uint16_t pt_idx = (virt >> 12) & 0x1FF;
    
    if ((pml4[pml4_idx] & PAGE_PRESENT) == 0) return 0;
    uint64_t *pdpt_ptr = (uint64_t *)(pml4[pml4_idx] & ~0xFFF);
    
    if ((pdpt_ptr[pdpt_idx] & PAGE_PRESENT) == 0) return 0;
    uint64_t *pd_ptr = (uint64_t *)(pdpt_ptr[pdpt_idx] & ~0xFFF);
    
    if ((pd_ptr[pd_idx] & PAGE_PRESENT) == 0) return 0;
    uint64_t *pt_ptr = (uint64_t *)(pd_ptr[pd_idx] & ~0xFFF);
    
    return pt_ptr[pt_idx] & ~0xFFF;
}
