.intel_syntax noprefix // 使用intel格式汇编语言

.section .text
switch_to:
    CLI
    // 栈中此处是RIP
    PUSH RSI // 56 ~ 63
    PUSH RDI // 48 ~ 55
    PUSH RBX // 40 ~ 47
    PUSH RBP // 32 ~ 39
    PUSH R12 // 24 ~ 31
    PUSH R13 // 16 ~ 23
    PUSH R14 // 8 ~ 15
    PUSH R15 // 0 ~ 7
    // 保存这个线程数据
    MOV [RDX], RSP
    MOV RSP, [RCX]
    // 切换到下一线程
    POP R15
    POP R14
    POP R13
    POP R12
    POP RBP
    POP RBX
    POP RDI
    POP RSI
    STI
    RETQ // 跳转
 
// 第一个参数存current,第二个参数存next
.GLOBAL main
main:
    MOV RCX, RSI
    MOV RDX, RDI
    JMP  switch_to