.intel_syntax noprefix // 使用intel格式汇编语言
.section .text
.GLOBAL _asm_thread_switch_to
_asm_thread_switch_to:
    CLI
    /* 栈中此处是RIP */
    PUSH RBP
    PUSH RSI
    PUSH RDI
    PUSH RBX
    PUSH R12
    PUSH R13
    PUSH R14
    PUSH R15
    MOVQ [RSP + 72], RAX
    MOVQ [RAX], RSP
    /* 保存这个进程数据 */
    /* 切换到下一线程 */
    MOVQ RAX, [RSP + 80]
    MOVQ RSP, [RAX]
    POP R15
    POP R14
    POP R13
    POP R12
    POP RBX
    POP RDI
    POP RSI
    POP RBP
    STI
    RET  /* 返回到上面switch_to下面的那句注释的RIP */
