.section .text
.global __libc_start_main
__libc_start_main:
    LEAQ _start(%RIP), %RAX
    CALL *%RAX
 

.extern main
_start:
    MOVQ $0, %RDI
    MOVQ $0, %RSI
    MOVQ $0, %RDX
    LEAQ main(%RIP), %RAX
    CALL *%RAX
    .1:
    JMP .1
    RET

