# Process Management and Scheduling Documentation

## Overview

The process management subsystem provides:
- Process creation and termination
- Context switching
- Priority-based scheduling
- Process statistics tracking

## Process States

```
┌─────────────┐
│  RUNNING    │ ← Currently executing
└──────┬──────┘
       │ yield/time slice
       ▼
┌─────────────┐
│   READY     │ ← Ready to run, waiting for CPU
└──────┬──────┘
       │ interrupt/wait for I/O
       ▼
┌─────────────┐
│  WAITING    │ ← Waiting for I/O or event
└──────┬──────┘
       │ resource available
       ▼
┌─────────────┐
│  BLOCKED    │ ← Blocked by lock/semaphore
└──────┬──────┘
       │ lock released
       ▼
┌─────────────┐
│TERMINATED   │ ← Process exited
└─────────────┘
```

## Priority Levels

| Level | Value | Usage |
|-------|-------|-------|
| LOW | 0 | Idle, background tasks |
| NORMAL | 1 | Regular user programs |
| HIGH | 2 | System services |
| CRITICAL | 3 | Kernel tasks, interrupts |

## Process Control Block (PCB)

```c
struct process_t {
    uint32_t pid;               // Process ID
    uint32_t parent_pid;        // Parent process ID
    process_state_t state;      // Current state
    process_priority_t priority;// Priority level
    cpu_context_t context;      // Saved CPU registers
    uint64_t stack_base;        // Stack base address
    uint64_t stack_size;        // Stack size in bytes
    uint64_t entry_point;       // Program entry address
    uint64_t time_slice;        // Remaining time slice
    uint64_t total_time;        // Total execution time
    uint32_t queue_index;       // Scheduler queue position
}
```

## Scheduling Algorithm

**Priority-based Round-Robin**

1. Check CRITICAL priority queue
2. Check HIGH priority queue
3. Check NORMAL priority queue
4. Check LOW priority queue
5. If all empty, run idle process

Each process gets DEFAULT_TIME_SLICE (10 ticks) per turn.

## CPU Context Structure

```c
struct cpu_context_t {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp;
    uint64_t r8-r15;
    uint64_t rip;       // Instruction pointer
    uint64_t rsp;       // Stack pointer
    uint64_t rflags;    // Flags
    uint64_t cr3;       // Page table base
}
```

## API Functions

### Process Management

```c
// Initialize process manager
void process_init(void);

// Create new process
uint32_t process_create(uint64_t entry_point, 
                        uint64_t stack_size, 
                        process_priority_t priority);

// Terminate process
void process_terminate(uint32_t pid);

// Switch between processes
void process_switch(uint32_t from_pid, uint32_t to_pid);

// Get process by PID
process_t *process_get(uint32_t pid);

// Set process state
void process_set_state(uint32_t pid, process_state_t state);

// Get current process PID
uint32_t process_get_current_pid(void);

// Yield CPU to scheduler
void process_yield(void);
```

### Scheduler

```c
// Initialize scheduler
void scheduler_init(void);

// Called on timer interrupt
void scheduler_tick(void);

// Get next process to run
uint32_t scheduler_next_process(void);
```

## Execution Flow

1. **Timer Interrupt (IRQ 0)**
   - scheduler_tick() called
   - Increment total_time
   - Decrement time_slice
   - If time_slice <= 0, call process_yield()

2. **Process Yield**
   - Get next process from scheduler
   - Call process_switch(current, next)
   - Save current context
   - Restore next context
   - Update current_pid

3. **Context Switch (Simplified)**
   - In real OS, would use `swapgs`, `mov cr3`, restore registers
   - Here: simulation with process state update

## Process Lifecycle

```
process_create()
    ↓
READY (in queue)
    ↓
scheduler_next_process() → RUNNING
    ↓
time slice expires → back to READY
    ↓
process_yield() / I/O wait → WAITING
    ↓
event/I/O complete → READY
    ↓
process_terminate() → TERMINATED
    ↓
Memory freed
```

## Limitations (Current Implementation)

- [ ] No actual CPU context switching (register restoration)
- [ ] No inter-process communication (IPC)
- [ ] No signals/exceptions handling
- [ ] No memory protection between processes
- [ ] No resource limits
- [ ] Simple scheduler (no dynamic priority adjustment)

## Future Enhancements

- [ ] Actual context switching assembly
- [ ] Message passing IPC
- [ ] Pipes and file descriptors
- [ ] Process groups and sessions
- [ ] Signal handling
- [ ] Resource quotas
- [ ] Multi-core scheduling
