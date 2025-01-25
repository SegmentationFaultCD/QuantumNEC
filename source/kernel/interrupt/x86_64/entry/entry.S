.intel_syntax noprefix // 使用intel格式汇编语言
#include <kernel/global/x86_64/selector.h>
.section .text
_ds =0
_es =8
_fs  =16
_gs  =24
_rax  =32
_rbx  =40
_rcs  =48
_rdx =56
_rbp  =64
_rsi   =72
_rdi   =80
_r8   =88
_r9   =96
_r10  =104
_r11  =112
_r12 =120
_r13 =128
_r14 =136
_r15 =144
_handler =152
_old_rax =160
_vector =168
_error_code =176
_rip =184
_cs =192
_rflags =200
_rsp =208
_ss =216

.macro TRAP_ENTRY n
.GLOBAL InterruptHandler\n
.endm


TRAP_ENTRY 0x00            /* #DE	除以0 */
TRAP_ENTRY 0x01            /* #DB	调试 */
TRAP_ENTRY 0x02            /* ---  NMI中断 */
TRAP_ENTRY 0x03            /* #BP	断点 */
TRAP_ENTRY 0x04            /* #OF  溢出 */
TRAP_ENTRY 0x05            /* #BR  数组索引超限 */
TRAP_ENTRY 0x06            /* #UD	未定义指令 */
TRAP_ENTRY 0x07            /* #NM	设备未就绪 */
TRAP_ENTRY 0x08            /* #DF	双重错误 */
TRAP_ENTRY 0x09            /* ---  协处理器错误 */
TRAP_ENTRY 0x0a            /* #TS	无效的TSS */
TRAP_ENTRY 0x0b            /* #NP	段不存在 */
TRAP_ENTRY 0x0c            /* #SS  堆栈段故障 */
TRAP_ENTRY 0x0d            /* #GP  一般保护故障 */
TRAP_ENTRY 0x0e            /* #PF	缺页 */
TRAP_ENTRY 0x0f            /* ---  intel保留，请勿使用 */
TRAP_ENTRY 0x10            /* #MF	x87浮点数异常 */
TRAP_ENTRY 0x11            /* #AC 内存数据引用未对齐 仅在Ring3触发 */
TRAP_ENTRY 0x12            /* #MC	处理器内部错误 */
TRAP_ENTRY 0x13            /* #XM/#XF	SIMD浮点数异常 */
TRAP_ENTRY 0x14            /* #VE 虚拟化异常 */
TRAP_ENTRY 0x15            /* #CP 控制保护异常 */
TRAP_ENTRY 0x16             
TRAP_ENTRY 0x17            
TRAP_ENTRY 0x18 
TRAP_ENTRY 0x19 
TRAP_ENTRY 0x1a 
TRAP_ENTRY 0x1b 
TRAP_ENTRY 0x1c             /* #HV 管理程序注入异常 */
TRAP_ENTRY 0x1d             /* #VC VMM 通信失败 */
TRAP_ENTRY 0x1e             /* #SX 安全异常 */
TRAP_ENTRY 0x1f 
/*0x00-0x1f	保护模式异常(Intel保留)*/

 
.macro INTERRUPT_ENTRY n                
.GLOBAL InterruptHandler\n
.section .text
InterruptHandler\n:
    CLI
    PUSH 0
    PUSH \n
    PUSH 0 // 跳过old_rax
    PUSH 0
    JMP save_all_registers
.endm
 
.section .text
 
.GLOBAL save_all_registers
save_all_registers:
    PUSH R15
    PUSH R14
    PUSH R13
    PUSH R12
    PUSH R11
    PUSH R10
    PUSH R9
    PUSH R8
    PUSH RDI
    PUSH RSI
    PUSH RBP
    PUSH RDX
    PUSH RCX
    PUSH RBX
    PUSH RAX 
    MOV  RAX, GS
    PUSH RAX
    MOV  RAX, FS
    PUSH RAX
    MOV  RAX, ES
    PUSH RAX
    MOV  RAX, DS
    PUSH RAX
    MOV  RDI, RSP
    CALL do_IRQ
.GLOBAL return_from_isr
return_from_isr:
    MOV RSP, RAX
    POP RAX
    MOV DS, RAX
    POP RAX
    MOV ES, RAX
    POP RAX
    // MOV FS, RAX
    POP RAX
    // MOV GS, RAX
    // ignore FS, GS

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

ret_from_exception:
    HLT
    JMP ret_from_exception

InterruptHandler0x00:
    CLI
    PUSH 0      // 压入错误码
    PUSH 0x00   // 压入vector
    PUSH RAX      // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
error_code:
    PUSH R15
    PUSH R14
    PUSH R13
    PUSH R12
    PUSH R11
    PUSH R10
    PUSH R9
    PUSH R8
    PUSH RDI
    PUSH RSI
    PUSH RBP
    PUSH RDX
    PUSH RCX
    PUSH RBX
    PUSH RAX 
    MOV  RAX, GS
    PUSH RAX
    MOV  RAX, FS
    PUSH RAX
    MOV  RAX, ES
    PUSH RAX
    MOV  RAX, DS
    PUSH RAX

    MOV	 RDI, SELECTOR_DATA64_KERNEL
	MOV	 DS, RDI
	MOV	 ES, RDI
    MOV RAX, [RSP + _old_rax] // 将rax还原，因为前面暂时用rax当作临时变量
    MOV [RSP + _rax], RAX
    MOV RDX, [RSP + _handler] // 得到处理函数头
    MOV RDI, RSP              // 将frame移到第一个参数
    CALL RDX
    JMP ret_from_exception
InterruptHandler0x01:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x01 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x02:
    CLI
    PUSH 0    // 压入错误码
    PUSH 0x02 // 压入编号
    PUSH 0    // 跳过old_rax（因为不需要）
    PUSH 0    // 不需要头函数就压0
    PUSH R15
    PUSH R14
    PUSH R13
    PUSH R12
    PUSH R11
    PUSH R10
    PUSH R9
    PUSH R8
    PUSH RDI
    PUSH RSI
    PUSH RBP
    PUSH RDX
    PUSH RCX
    PUSH RBX
    PUSH RAX 
    MOV  RAX, GS
    PUSH RAX
    MOV  RAX, FS
    PUSH RAX
    MOV  RAX, ES
    PUSH RAX
    MOV  RAX, DS
    PUSH RAX
    MOV	 RDX, SELECTOR_DATA64_KERNEL
	MOV	 DS, RDX
	MOV	 ES, RDX
    MOV RDI, RSP
    CALL do_IRQ
    JMP return_from_isr
InterruptHandler0x03:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x03 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x04:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x04 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x05:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x05 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x06:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x06 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x07:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x07 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x08:
    CLI
    NOP // 压入错误码
    PUSH 0x08 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x09:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x09 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x0a:
    CLI
    NOP // 压入错误码
    PUSH 0x0a // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x0b:
    CLI
    NOP // 压入错误码
    PUSH 0x0b // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x0c:
    CLI
    NOP // 压入错误码
    PUSH 0x0c // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x0d:
    CLI
    NOP // 压入错误码
    PUSH 0x0d // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x0e:
    CLI
    NOP // 压入错误码
    PUSH 0x0e // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code

// Intel保留
InterruptHandler0x0f:
    JMP ret_from_exception

InterruptHandler0x10:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x10 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code


InterruptHandler0x11:
    CLI
    NOP // 压入错误码
    PUSH 0x11 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x12:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x12 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x13:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x13 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x14:
    CLI
    PUSH 0 // 压入错误码
    PUSH 0x14 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x15:
    CLI
    NOP // 压入错误码
    PUSH 0x15 // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x16:
    JMP ret_from_exception
InterruptHandler0x17:
    JMP ret_from_exception
InterruptHandler0x18:
    JMP ret_from_exception
InterruptHandler0x19:
    JMP ret_from_exception
InterruptHandler0x1a:
    JMP ret_from_exception
InterruptHandler0x1b:
    JMP ret_from_exception
InterruptHandler0x1c:
    CLI
    NOP // 压入错误码
    PUSH 0x1c // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x1d:
    CLI
    NOP // 压入错误码
    PUSH 0x1d // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XCHG [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x1e:
    CLI
    NOP // 压入错误码
    PUSH 0x1d // 压入编号
    PUSH RAX    // 把rax压入栈
    PUSH RAX    
	LEA	 RAX, [RIP + do_IRQ]
	XOR [RSP],	RAX  // 将头函数压入
    XOR RAX, RAX
    JMP error_code
InterruptHandler0x1f:
    JMP ret_from_exception
INTERRUPT_ENTRY 0x20       /* 时间中断 */
INTERRUPT_ENTRY 0x21       /* 键盘中断 */
INTERRUPT_ENTRY 0x22       /* 级联(两个芯片内部使用。不会发起)/HPET timer 0/8254 counter 0 */
INTERRUPT_ENTRY 0x23       /* 串口COM2&4对应的入口 */
INTERRUPT_ENTRY 0x24       /* 串口COM1&3对应的入口*/
INTERRUPT_ENTRY 0x25       /* 并口LPT2对应的入口 */
INTERRUPT_ENTRY 0x26       /* 软盘对应的入口 */
INTERRUPT_ENTRY 0x27       /* 并口LPT1对应的入口 [通常是“伪”中断（不可靠）] */
INTERRUPT_ENTRY 0x28       /* CMOS实时时钟/HPET timer 1 */
INTERRUPT_ENTRY 0x29       /* 自由外设/legacy SCSI/网卡 */
INTERRUPT_ENTRY 0x2a       /* 自由外设/SCSI/网卡 */
INTERRUPT_ENTRY 0x2b       /* HPET timer 2/自由外设/SCSI/网卡  */
INTERRUPT_ENTRY 0x2c       /* HPET timer 3/PS2鼠标接口 */
INTERRUPT_ENTRY 0x2d       /* FPU/协处理器/间处理器 */
INTERRUPT_ENTRY 0x2e       /* 主ATA硬盘 */
INTERRUPT_ENTRY 0x2f       /* 从ATA硬盘 */
INTERRUPT_ENTRY 0x30       
INTERRUPT_ENTRY 0x31       
INTERRUPT_ENTRY 0x32 
INTERRUPT_ENTRY 0x33      /* APIC ERROR */
INTERRUPT_ENTRY 0x34 
INTERRUPT_ENTRY 0x35 
INTERRUPT_ENTRY 0x36 
INTERRUPT_ENTRY 0x37 
INTERRUPT_ENTRY 0x38 
INTERRUPT_ENTRY 0x39 
INTERRUPT_ENTRY 0x3a 
INTERRUPT_ENTRY 0x3b 
INTERRUPT_ENTRY 0x3c 
INTERRUPT_ENTRY 0x3d 
INTERRUPT_ENTRY 0x3e 
INTERRUPT_ENTRY 0x3f     /* APIC SPURIOUS */
INTERRUPT_ENTRY 0x40 
INTERRUPT_ENTRY 0x41 
INTERRUPT_ENTRY 0x42 
INTERRUPT_ENTRY 0x43 
INTERRUPT_ENTRY 0x44 
INTERRUPT_ENTRY 0x45 
INTERRUPT_ENTRY 0x46 
INTERRUPT_ENTRY 0x47 
INTERRUPT_ENTRY 0x48 
INTERRUPT_ENTRY 0x49 
INTERRUPT_ENTRY 0x4a 
INTERRUPT_ENTRY 0x4b 
INTERRUPT_ENTRY 0x4c 
INTERRUPT_ENTRY 0x4d 
INTERRUPT_ENTRY 0x4e 
INTERRUPT_ENTRY 0x4f 
INTERRUPT_ENTRY 0x50 
INTERRUPT_ENTRY 0x51 
INTERRUPT_ENTRY 0x52 
INTERRUPT_ENTRY 0x53 
INTERRUPT_ENTRY 0x54 
INTERRUPT_ENTRY 0x55 
INTERRUPT_ENTRY 0x56 
INTERRUPT_ENTRY 0x57 
INTERRUPT_ENTRY 0x58 
INTERRUPT_ENTRY 0x59 
INTERRUPT_ENTRY 0x5a 
INTERRUPT_ENTRY 0x5b 
INTERRUPT_ENTRY 0x5c 
INTERRUPT_ENTRY 0x5d 
INTERRUPT_ENTRY 0x5e 
INTERRUPT_ENTRY 0x5f 
INTERRUPT_ENTRY 0x60 
INTERRUPT_ENTRY 0x61 
INTERRUPT_ENTRY 0x62 
INTERRUPT_ENTRY 0x63 
INTERRUPT_ENTRY 0x64 
INTERRUPT_ENTRY 0x65 
INTERRUPT_ENTRY 0x66 
INTERRUPT_ENTRY 0x67 
INTERRUPT_ENTRY 0x68 
INTERRUPT_ENTRY 0x69 
INTERRUPT_ENTRY 0x6a 
INTERRUPT_ENTRY 0x6b 
INTERRUPT_ENTRY 0x6c 
INTERRUPT_ENTRY 0x6d 
INTERRUPT_ENTRY 0x6e 
INTERRUPT_ENTRY 0x6f 
INTERRUPT_ENTRY 0x70 
INTERRUPT_ENTRY 0x71 
INTERRUPT_ENTRY 0x72 
INTERRUPT_ENTRY 0x73 
INTERRUPT_ENTRY 0x74 
INTERRUPT_ENTRY 0x75 
INTERRUPT_ENTRY 0x76 
INTERRUPT_ENTRY 0x77 
INTERRUPT_ENTRY 0x78 
INTERRUPT_ENTRY 0x79 
INTERRUPT_ENTRY 0x7a 
INTERRUPT_ENTRY 0x7b 
INTERRUPT_ENTRY 0x7c 
INTERRUPT_ENTRY 0x7d 
INTERRUPT_ENTRY 0x7e 
INTERRUPT_ENTRY 0x7f 
INTERRUPT_ENTRY 0x80      /* 系统调用 */
INTERRUPT_ENTRY 0x81 
INTERRUPT_ENTRY 0x82 
INTERRUPT_ENTRY 0x83 
INTERRUPT_ENTRY 0x84 
INTERRUPT_ENTRY 0x85 
INTERRUPT_ENTRY 0x86 
INTERRUPT_ENTRY 0x87 
INTERRUPT_ENTRY 0x88 
INTERRUPT_ENTRY 0x89 
INTERRUPT_ENTRY 0x8a 
INTERRUPT_ENTRY 0x8b 
INTERRUPT_ENTRY 0x8c 
INTERRUPT_ENTRY 0x8d 
INTERRUPT_ENTRY 0x8e 
INTERRUPT_ENTRY 0x8f 
INTERRUPT_ENTRY 0x90 
INTERRUPT_ENTRY 0x91 
INTERRUPT_ENTRY 0x92 
INTERRUPT_ENTRY 0x93 
INTERRUPT_ENTRY 0x94 
INTERRUPT_ENTRY 0x95 
INTERRUPT_ENTRY 0x96 
INTERRUPT_ENTRY 0x97 
INTERRUPT_ENTRY 0x98 
INTERRUPT_ENTRY 0x99 
INTERRUPT_ENTRY 0x9a 
INTERRUPT_ENTRY 0x9b 
INTERRUPT_ENTRY 0x9c 
INTERRUPT_ENTRY 0x9d 
INTERRUPT_ENTRY 0x9e 
INTERRUPT_ENTRY 0x9f 
INTERRUPT_ENTRY 0xa0 
INTERRUPT_ENTRY 0xa1 
INTERRUPT_ENTRY 0xa2 
INTERRUPT_ENTRY 0xa3 
INTERRUPT_ENTRY 0xa4 
INTERRUPT_ENTRY 0xa5 
INTERRUPT_ENTRY 0xa6 
INTERRUPT_ENTRY 0xa7 
INTERRUPT_ENTRY 0xa8 
INTERRUPT_ENTRY 0xa9 
INTERRUPT_ENTRY 0xaa 
INTERRUPT_ENTRY 0xab 
INTERRUPT_ENTRY 0xac 
INTERRUPT_ENTRY 0xad 
INTERRUPT_ENTRY 0xae 
INTERRUPT_ENTRY 0xaf 
INTERRUPT_ENTRY 0xb0 
INTERRUPT_ENTRY 0xb1 
INTERRUPT_ENTRY 0xb2 
INTERRUPT_ENTRY 0xb3 
INTERRUPT_ENTRY 0xb4 
INTERRUPT_ENTRY 0xb5 
INTERRUPT_ENTRY 0xb6 
INTERRUPT_ENTRY 0xb7 
INTERRUPT_ENTRY 0xb8 
INTERRUPT_ENTRY 0xb9 
INTERRUPT_ENTRY 0xba 
INTERRUPT_ENTRY 0xbb 
INTERRUPT_ENTRY 0xbc 
INTERRUPT_ENTRY 0xbd 
INTERRUPT_ENTRY 0xbe 
INTERRUPT_ENTRY 0xbf 
INTERRUPT_ENTRY 0xc0 
INTERRUPT_ENTRY 0xc1 
INTERRUPT_ENTRY 0xc2 
INTERRUPT_ENTRY 0xc3 
INTERRUPT_ENTRY 0xc4 
INTERRUPT_ENTRY 0xc5 
INTERRUPT_ENTRY 0xc6 
INTERRUPT_ENTRY 0xc7 
INTERRUPT_ENTRY 0xc8 
INTERRUPT_ENTRY 0xc9 
INTERRUPT_ENTRY 0xca 
INTERRUPT_ENTRY 0xcb 
INTERRUPT_ENTRY 0xcc 
INTERRUPT_ENTRY 0xcd 
INTERRUPT_ENTRY 0xce 
INTERRUPT_ENTRY 0xcf 
INTERRUPT_ENTRY 0xd0 
INTERRUPT_ENTRY 0xd1 
INTERRUPT_ENTRY 0xd2 
INTERRUPT_ENTRY 0xd3 
INTERRUPT_ENTRY 0xd4 
INTERRUPT_ENTRY 0xd5 
INTERRUPT_ENTRY 0xd6 
INTERRUPT_ENTRY 0xd7 
INTERRUPT_ENTRY 0xd8 
INTERRUPT_ENTRY 0xd9 
INTERRUPT_ENTRY 0xda 
INTERRUPT_ENTRY 0xdb 
INTERRUPT_ENTRY 0xdc 
INTERRUPT_ENTRY 0xdd 
INTERRUPT_ENTRY 0xde 
INTERRUPT_ENTRY 0xdf 
INTERRUPT_ENTRY 0xe0 
INTERRUPT_ENTRY 0xe1 
INTERRUPT_ENTRY 0xe2 
INTERRUPT_ENTRY 0xe3 
INTERRUPT_ENTRY 0xe4 
INTERRUPT_ENTRY 0xe5 
INTERRUPT_ENTRY 0xe6 
INTERRUPT_ENTRY 0xe7 
INTERRUPT_ENTRY 0xe8 
INTERRUPT_ENTRY 0xe9 
INTERRUPT_ENTRY 0xea 
INTERRUPT_ENTRY 0xeb 
INTERRUPT_ENTRY 0xec 
INTERRUPT_ENTRY 0xed 
INTERRUPT_ENTRY 0xee 
INTERRUPT_ENTRY 0xef 
INTERRUPT_ENTRY 0xf0 
INTERRUPT_ENTRY 0xf1 
INTERRUPT_ENTRY 0xf2 
INTERRUPT_ENTRY 0xf3 
INTERRUPT_ENTRY 0xf4 
INTERRUPT_ENTRY 0xf5 
INTERRUPT_ENTRY 0xf6 
INTERRUPT_ENTRY 0xf7 
INTERRUPT_ENTRY 0xf8 
INTERRUPT_ENTRY 0xf9 
INTERRUPT_ENTRY 0xfa 
INTERRUPT_ENTRY 0xfb 
INTERRUPT_ENTRY 0xfc 
INTERRUPT_ENTRY 0xfd 
INTERRUPT_ENTRY 0xfe 
INTERRUPT_ENTRY 0xff 

.GLOBAL system_call


.EXTERN get_current_kernel_stack_top_in_user_mode
// We will use rax, rdi，rsi，rdx，r10，r8，and r9, so these register will be saved
// Five state will be saved, too
.GLOBAL system_call
system_call:
    CLI
    // save into user stack
    PUSH RBP  //                0x60

    PUSH RSP  // user RSP       0x58
    PUSH SELECTOR_DATA64_USER // user SS        0x50
    PUSH SELECTOR_CODE64_USER // user CS        0x48
    PUSH RCX  // rip            0x40
    PUSH R11  // rflags         0x38
    
    PUSH RAX  //                0x30
    PUSH RDI  //                0x28
    PUSH RSI  //                0x20
    PUSH RDX  //                0x18
    PUSH R10  //                0x10
    PUSH R9   //                0x08
    PUSH R8   //                0x00
        
    MOV RBP, RSP
    // switch into kernel stack
    LEA RAX, [get_current_kernel_stack_top_in_user_mode + RIP]
    CALL RAX
    MOV RSP, RAX

    
    PUSH [RBP + 0x50]
    PUSH [RBP + 0x58]
    PUSH [RBP + 0x38]
    PUSH [RBP + 0x48]
    PUSH [RBP + 0x40]
    PUSH 0
    PUSH 0x80
    PUSH 0
    PUSH 0
    PUSH 0
    PUSH 0
    PUSH 0
    PUSH 0
    PUSH 0
    PUSH [RBP + 0x10]
    PUSH [RBP + 0x8]
    PUSH [RBP]
    PUSH [RBP + 0x28]
    PUSH [RBP + 0x20]
    PUSH [RBP + 0x60]
    PUSH [RBP + 0x18]
    PUSH 0
    PUSH 0
    PUSH [RBP + 0x30] 
    MOV  RAX, GS
    PUSH RAX
    MOV  RAX, FS
    PUSH RAX
    MOV  RAX, ES
    PUSH RAX
    MOV  RAX, DS
    PUSH RAX
    MOV	 RDI, SELECTOR_DATA64_KERNEL
	MOV	 DS, RDI
	MOV	 ES, RDI
    MOV  RDI, RSP
    LEA  RAX, [do_IRQ + RIP]
    CALL RAX
    MOV RAX, [RAX + _rax]
    // switch into user stack
    MOV RSP, RBP
    POP R8
    POP R9
    POP R10
    POP RDX
    POP RSI
    POP RDI
    ADD RSP, 8 // return value is saved in rax
    POP R11 // rflags
    POP RCX // rip
    ADD RSP, 0x18
    // OSDEV says : Note that the Kernel does not automatically have a kernel stack loaded. This is the handler's responsibility.
    // So, we should switch to kernel stack by ourselves
    POP RBP
    STI
    SYSRETQ
.section .data

