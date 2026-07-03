/* Process Management Implementation */

#include "process.h"
#include "memory.h"
#include "console.h"
#include "drivers.h"

/* Maximum number of processes */
#define MAX_PROCESSES 256
#define DEFAULT_TIME_SLICE 10

/* Process table */
static process_t process_table[MAX_PROCESSES];
static uint32_t process_count = 0;
static uint32_t next_pid = 1;
static uint32_t current_pid = 0;

/* Scheduler queues - one per priority level */
typedef struct {
    uint32_t pids[MAX_PROCESSES];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} process_queue_t;

static process_queue_t ready_queues[4];  /* One for each priority level */
static uint64_t total_context_switches = 0;

/* Initialize process management */
void process_init(void)
{
    terminal_write_string("[PROCESS] Initializing process management...\n");
    
    /* Clear process table */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = 0;
        process_table[i].state = PROCESS_TERMINATED;
    }
    
    /* Clear scheduler queues */
    for (int i = 0; i < 4; i++) {
        ready_queues[i].head = 0;
        ready_queues[i].tail = 0;
        ready_queues[i].count = 0;
    }
    
    /* Create idle process */
    process_create(0, 4096, PRIORITY_LOW);
    
    terminal_write_string("[PROCESS] Process management initialized\n");
}

/* Create a new process */
uint32_t process_create(uint64_t entry_point, uint64_t stack_size, process_priority_t priority)
{
    if (process_count >= MAX_PROCESSES) {
        terminal_write_string("[PROCESS] Maximum processes reached!\n");
        return 0;
    }
    
    /* Allocate process slot */
    process_t *proc = &process_table[process_count];
    proc->pid = next_pid++;
    proc->parent_pid = current_pid;
    proc->state = PROCESS_READY;
    proc->priority = priority;
    proc->entry_point = entry_point;
    proc->stack_size = stack_size;
    proc->stack_base = (uint64_t)malloc(stack_size);
    proc->time_slice = DEFAULT_TIME_SLICE;
    proc->total_time = 0;
    proc->queue_index = 0;
    
    /* Initialize CPU context */
    proc->context.rip = entry_point;
    proc->context.rsp = proc->stack_base + stack_size;
    proc->context.rflags = 0x202;  /* IF flag set */
    
    /* Add to ready queue */
    process_queue_t *queue = &ready_queues[priority];
    queue->pids[queue->tail] = proc->pid;
    queue->tail = (queue->tail + 1) % MAX_PROCESSES;
    queue->count++;
    
    process_count++;
    
    terminal_write_string("[PROCESS] Created process PID=");
    print_number(proc->pid);
    terminal_write_string(" (priority=");
    print_number(priority);
    terminal_write_string(")\n");
    
    return proc->pid;
}

/* Terminate a process */
void process_terminate(uint32_t pid)
{
    for (uint32_t i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].state = PROCESS_TERMINATED;
            free((void *)process_table[i].stack_base);
            
            terminal_write_string("[PROCESS] Terminated process PID=");
            print_number(pid);
            terminal_write_string("\n");
            return;
        }
    }
}

/* Switch context between processes */
void process_switch(uint32_t from_pid, uint32_t to_pid)
{
    process_t *from_proc = process_get(from_pid);
    process_t *to_proc = process_get(to_pid);
    
    if (from_proc == NULL || to_proc == NULL) {
        return;
    }
    
    /* Save current context (in real implementation, would save CPU registers) */
    from_proc->state = PROCESS_READY;
    
    /* Restore new context (in real implementation, would restore CPU registers) */
    to_proc->state = PROCESS_RUNNING;
    current_pid = to_pid;
    total_context_switches++;
}

/* Get process by PID */
process_t *process_get(uint32_t pid)
{
    for (uint32_t i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return NULL;
}

/* Set process state */
void process_set_state(uint32_t pid, process_state_t state)
{
    process_t *proc = process_get(pid);
    if (proc != NULL) {
        proc->state = state;
    }
}

/* Get current process PID */
uint32_t process_get_current_pid(void)
{
    return current_pid;
}

/* Yield CPU to next process */
void process_yield(void)
{
    uint32_t next = scheduler_next_process();
    if (next != current_pid) {
        process_switch(current_pid, next);
    }
}

/* ==================== SCHEDULER ==================== */

/* Initialize scheduler */
void scheduler_init(void)
{
    terminal_write_string("[SCHEDULER] Initializing scheduler...\n");
    process_init();
    terminal_write_string("[SCHEDULER] Scheduler initialized\n");
}

/* Called on each timer tick */
void scheduler_tick(void)
{
    process_t *current = process_get(current_pid);
    
    if (current != NULL) {
        current->total_time++;
        current->time_slice--;
        
        /* Time slice expired, schedule next process */
        if (current->time_slice <= 0) {
            current->time_slice = DEFAULT_TIME_SLICE;
            process_yield();
        }
    }
}

/* Get next process to run */
uint32_t scheduler_next_process(void)
{
    /* Priority-based scheduling: check higher priority queues first */
    for (int priority = PRIORITY_CRITICAL; priority >= PRIORITY_LOW; priority--) {
        process_queue_t *queue = &ready_queues[priority];
        
        if (queue->count > 0) {
            uint32_t pid = queue->pids[queue->head];
            queue->head = (queue->head + 1) % MAX_PROCESSES;
            queue->count--;
            
            /* Re-add to queue for round-robin */
            queue->pids[queue->tail] = pid;
            queue->tail = (queue->tail + 1) % MAX_PROCESSES;
            queue->count++;
            
            return pid;
        }
    }
    
    /* Return idle process if nothing ready */
    return 1;
}

/* Get process statistics */
void process_stats(void)
{
    terminal_write_string("\n=== Process Statistics ===\n");
    terminal_write_string("Total Processes: ");
    print_number(process_count);
    terminal_write_string("\n");
    
    terminal_write_string("Context Switches: ");
    print_number(total_context_switches);
    terminal_write_string("\n");
    
    terminal_write_string("Current PID: ");
    print_number(current_pid);
    terminal_write_string("\n");
    
    terminal_write_string("Process List:\n");
    for (uint32_t i = 0; i < process_count; i++) {
        terminal_write_string("  PID=");
        print_number(process_table[i].pid);
        terminal_write_string(" State=");
        print_number(process_table[i].state);
        terminal_write_string(" Time=");
        print_number(process_table[i].total_time);
        terminal_write_string("\n");
    }
    
    terminal_write_string("==========================\n\n");
}
