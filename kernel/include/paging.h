/* Paging and Virtual Memory */

#ifndef __PAGING_H__
#define __PAGING_H__

#include "types.h"

/* Paging initialization */
void paging_init(void);

/* Enable paging */
void enable_paging(void);

/* Page allocation */
uint64_t page_alloc(void);
void page_free(uint64_t page);

/* Page mapping */
void page_map(uint64_t virt, uint64_t phys, uint64_t flags);
void page_unmap(uint64_t virt);
uint64_t page_get_physical(uint64_t virt);

#endif /* __PAGING_H__ */
