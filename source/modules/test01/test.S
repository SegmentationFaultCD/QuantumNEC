

.section .data
re = 0x7FFFFFFFF123


.section .code64
.section .text

.global _start
_start:
    nop
    nop
    nop
    nop
    movq $0x7FFFFFFFF123, %rax
    pushq %rax
    call _rrr
    popq %rax
    jmp _start
_rrr:
    retq