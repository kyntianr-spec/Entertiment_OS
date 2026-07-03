; Exception handlers for interrupts
; Each exception pushes interrupt number and calls common handler

[BITS 64]

extern interrupt_handler_wrapper

; Macro for exceptions without error code
%macro EXCEPTION 1
    global exception_%1
    exception_%1:
        push 0                  ; Push fake error code
        push %1                 ; Push exception number
        jmp .exception_common
%endmacro

; Macro for exceptions with error code
%macro EXCEPTION_ERROR 1
    global exception_%1
    exception_%1:
        push %1                 ; Push exception number
        jmp .exception_common
%endmacro

; Define all exception handlers
EXCEPTION 0
EXCEPTION 1
EXCEPTION 2
EXCEPTION 3
EXCEPTION 4
EXCEPTION 5
EXCEPTION 6
EXCEPTION 7
EXCEPTION_ERROR 8
EXCEPTION 9
EXCEPTION_ERROR 10
EXCEPTION_ERROR 11
EXCEPTION_ERROR 12
EXCEPTION_ERROR 13
EXCEPTION_ERROR 14
EXCEPTION 15
EXCEPTION 16
EXCEPTION_ERROR 17
EXCEPTION 18
EXCEPTION 19
EXCEPTION 20

.exception_common:
    ; Save all registers
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    
    ; Call C handler with pointer to frame
    mov rdi, rsp
    call interrupt_handler_wrapper
    
    ; Restore all registers
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    
    ; Skip error code and exception number
    add rsp, 16
    
    iretq
