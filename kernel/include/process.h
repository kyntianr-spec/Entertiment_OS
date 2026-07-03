/* Process Management */

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "types.h"

/* Process states */
typedef enum {
    PROCESS_RUNNING = 0,
    PROCESS_READY = 1,
    PROCESS_WAITING = 2,
    PROCESS_BLOCKED = 3,
    PROCESS_TERMINATED = 4
} process_state_t;

/* Process priority levels */
typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_CRITICAL = 3
} process_priority_t;

/* CPU context for context switching */
typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, rsp, rflags;
    uint64_t cr3;  /* Page table base */
} cpu_context_t;

/* Process Control Block (PCB) */
typedef struct {
    uint32_t pid;               /* Process ID */
    uint32_t parent_pid;        /* Parent process ID */
    process_state_t state;      /* Current state */
    process_priority_t priority;/* Priority level */
    cpu_context_t context;      /* CPU context */
    uint64_t stack_base;        /* Stack base address */
    uint64_t stack_size;        /* Stack size */
    uint64_t entry_point;       /* Entry point address */
    uint64_t time_slice;        /* Time slice (ticks) */
    uint64_t total_time;        /* Total execution time */
    uint32_t queue_index;       /* Position in scheduler queue */
} process_t;

/* Process management functions */
void process_init(void);
uint32_t process_create(uint64_t entry_point, uint64_t stack_size, process_priority_t priority);
void process_terminate(uint32_t pid);
void process_switch(uint32_t from_pid, uint32_t to_pid);
process_t *process_get(uint32_t pid);
void process_set_state(uint32_t pid, process_state_t state);
uint32_t process_get_current_pid(void);
void process_yield(void);

/* Scheduler functions */
void scheduler_init(void);
void scheduler_tick(void);
uint32_t scheduler_next_process(void);

#endif /* __PROCESS_H__ */
