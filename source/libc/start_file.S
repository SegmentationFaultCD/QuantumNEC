.section .text
.global __libc_start_main
__libc_start_main:
    LEAQ _start(%RIP), %RAX
    CALL *%RAX
loop:
    JMP loop

_start:
    MOVQ $0, %RDI
    MOVQ $0, %RSI
    MOVQ $0, %RDX
    LEAQ main(%RIP), %RAX
    CALL *%RAX
    RET
