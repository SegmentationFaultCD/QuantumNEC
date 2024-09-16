.intel_syntax noprefix // 使用intel格式汇编语言

.section .text

to_process: 
    POP RAX
    MOV DS, RAX
    POP RAX
    MOV ES, RAX
    POP RAX
    MOV FS, RAX
    POP RAX
    MOV GS, RAX
    POP RAX
    POP RBX
    POP RCX
    POP RDX
    POP RBP
    POP RSI
    POP RDI
    POP R8
    POP R9
    POP R10
    POP R11
    POP R12
    POP R13
    POP R14
    POP R15
    ADD RSP, 32 
    IRETQ
.GLOBAL main
main:
    MOV RSP, RDI
    JMP to_process