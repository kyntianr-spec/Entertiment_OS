# Memory Management Documentation

## Overview

The Entertiment OS memory management system provides:
- Physical memory tracking
- Virtual memory with paging
- Heap allocation (malloc/free)
- Memory protection

## Components

### 1. Physical Memory Management

- Tracks total system memory
- Manages memory regions
- Provides statistics

### 2. Virtual Memory (Paging)

Implements 4-level page table structure:

```
PML4 (Level 1) -> PDPT (Level 2) -> PD (Level 3) -> PT (Level 4)
   64-bit         64-bit            64-bit           64-bit
```

#### Memory Layout

- **0x000000 - 0x0FFFFF**: Real Mode IVT, BIOS Data
- **0x100000 - 0x2FFFFF**: Kernel (2MB)
- **0x300000+**: User programs
- **0xB8000**: VGA Memory

### 3. Heap Allocator

- Linear allocator (simple)
- 10 MB heap size
- 16-byte alignment
- malloc() / free() support

## Page Flags

| Flag | Value | Meaning |
|------|-------|---------|
| PRESENT | 0x01 | Page is in memory |
| WRITE | 0x02 | Page is writable |
| USER | 0x04 | User-mode accessible |
| ACCESSED | 0x20 | Page was accessed |
| DIRTY | 0x40 | Page was modified |

## Functions

### Initialization
```c
void memory_init(void);
void paging_init(void);
void enable_paging(void);
```

### Memory Operations
```c
void *malloc(size_t size);
void free(void *ptr);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
```

### Paging Operations
```c
uint64_t page_alloc(void);
void page_free(uint64_t page);
void page_map(uint64_t virt, uint64_t phys, uint64_t flags);
void page_unmap(uint64_t virt);
uint64_t page_get_physical(uint64_t virt);
```

## Build

```bash
make kernel
```

## Future Improvements

- [ ] Page bitmap management
- [ ] Physical memory allocation
- [ ] Copy-on-write pages
- [ ] Demand paging
- [ ] Memory protection (per-process)
- [ ] Swap space management
