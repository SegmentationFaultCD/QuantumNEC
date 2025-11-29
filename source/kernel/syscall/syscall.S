.intel_syntax noprefix // 使用intel格式汇编语言

.EXTERN get_kstack
.EXTERN system_call

.GLOBAL system_call_entry
system_call_entry:

    // rax, rdi, rsi, rdx, r10, r8, r9, rcx, r11这几个不能动
    // r11 -> rflags
    // rcx -> rip
    
    mov r13, rax
    mov r14, rcx
    mov r15, rdx
 
    swapgs
    mov gs:[0x08], rsp
    mov rsp, gs:[0x00]
   
    push 0x1b
    push gs:[0x08]
    push r11
    push 0x23
    push r14
    push 0
    push 0x80
    push 0
    push 0
    push 0
    push r12
    push 0
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push r15
    push 0
    push 0
    push r13 
    mov  rax, gs
    push rax
    mov  rax, fs
    push rax
    mov  rax, es
    push rax
    mov  rax, ds
    push rax
    mov rdi, rsp
    call system_call  
    mov rsp, rax
    add rsp, 32
    pop rax
    pop rbx
    add rsp, 8
    pop rdx
    pop rbp
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    add rsp, 8
    pop r12
    pop r13
    pop r14
    pop r15
    add rsp, 16
    pop rcx // rip
    add rsp, 8
    pop r11 // rflags
    pop rsp
    
    swapgs

    sysretq
    