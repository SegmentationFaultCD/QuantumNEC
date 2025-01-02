

.section .data



.section .code64
.section .text

.global _start
_start:
    nop
    nop
    nop 
    nop
    push %rax
    push %rbx
    push %rcx
    push %rdx
    call _rrr
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    movq $0, %rax
    movq $1, %rdi
    movq $0x114514, %rsi
    syscall
.1:
    jmp .1
_rrr:
    retq