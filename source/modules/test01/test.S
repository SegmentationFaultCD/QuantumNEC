

.section .data



.section .code64
.section .text

.global _start
_start:
    nop
    nop
    nop 
    nop
    syscall
    jmp _start
_rrr:
    retq