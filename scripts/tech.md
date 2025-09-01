# 自制OS教程#0 : 预备

archlinux: gcc(clang) nasm

# 自制OS教程#1 : 步入bootloader的大门

~~这里什么也没有~~  

什么，你问我怎么写引导?  
手写MBR?  
借助GNU-EFI或者EDK2这样的强大工具编写UEFI引导?  
或者是GRUB?  
今天让我帮你脱离苦海😋  
LIMINE!永远真神!  
放弃困难，选择limine!  
阿门!  

这里有limine的介绍，感兴趣的可以看看:[limine explain](https://github.com/limine-bootloader/limine)  

~~首先，在你自己的os源码文件夹下面从github拉取limine~~  
你可以用我们社区提供的[Limine C Xmake Template](https://github.com/plos-clan/limine-xmake-template)引导模板即可  

直接像这样操作:<br>
```bash
git clone https://github.com/plos-clan/limine-xmake-template.git
```

你可能会问: 什么,如此简单?  
是的是的你说的对,确实如此简单  

接下来要干什么?  
~~特么仓库readme.md不看么~~  
嗯，你就可以在src/main.c开始你的旅程辣!  

编译指令：在工作目录下面输入: 
```bash
xmake
```
编译源码即可  

是的你没有看错, 是[xmake](https://xmake.io/guide/quick-start.html), 不是煞笔一般的cmake或者make.  
不需要你写一大坨的CMakelist, 不需要你一个一个文件夹写Makefile, 一个xmake.lua足矣.  

没了?  
是的，bootloader就这样结束了,一个limine就搞定, 简单, 便利, 没有繁杂的所谓"把boot.bin写到0号扇区, 前512个字节我们要切到保护模式, lba读盘, 找到loader.bin, 切换长模式, VBE, 预备页表...","使用UEFI提供的Protocol获取acpi表, 得到内核地址并解析elf文件,重定位,获取graphic frame, 拿到efi memory map..."之类的东西.
如果我们要用到一些启动时的信息, 比如memory map, 我们只需要在源文件里面这样做:
```c
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 3,
    .response = nullptr
};
```
其中,revision视情况而定, 你使用的limine版本是多少就填多少.  
- 注意: 第三代limine不默认映射0~4GB物理地址到HHDM, 只映射部分, 后期我们会讲到, 这里先打个预防针


# 自制OS教程#2 : 串口, 启动!
为啥要先写串口,你先别急,我后面会慢慢告诉你原因.  
串口的文章可以在osdev上找到: [serial_port](https://wiki.osdev.org/Serial_Ports)  
在此之前, 我们需要实现这么一些函数:io_in8, io_out8, io_in16, io_out16, io_in32, io_out32  
这些函数是我们进行io操作的一个基础, 当然以后学了dma, mmio之类的东西你会更能理解他们的作用了.  
C版本
```c
static inline uint8_t io_in8(uint16_t port) {
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline uint16_t io_in16(uint16_t port) {
    uint16_t data;
    __asm__ volatile("inw %w1, %w0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline uint32_t io_in32(uint16_t port) {
    uint32_t data;
    __asm__ volatile("inl %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void io_out8(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static inline void io_out16(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %w0, %w1" : : "a"(data), "Nd"(port));
}

static inline void io_out32(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}
```
C++版本可以看这里：[io.hpp](https://github.com/SegmentationFaultCD/QuantumNEC/blob/limine/include/kernel/driver/cpu/io.hpp)以及[io.cpp](https://github.com/SegmentationFaultCD/QuantumNEC/blob/limine/source/kernel/driver/cpu/io.cpp)  

- uint32_t这些都是一些标量的typedef, 你include一下stdint.h就行了

接着按照osdev教的办法初始化串口, 如下(感觉我都没必要贴代码)
```c
#define PORT 0x3f8

static int support_serial_ports;

static int init_serial() {
   io_out8(PORT + 1, 0x00);    
   io_out8(PORT + 3, 0x80);   
   io_out8(PORT + 0, 0x03);   
   io_out8(PORT + 1, 0x00);    
   io_out8(PORT + 3, 0x03);  
   io_out8(PORT + 2, 0xC7);    
   io_out8(PORT + 4, 0x0B); 
   io_out8(PORT + 4, 0x1E);    
   io_out8(PORT + 0, 0xAE);  
    if ( IO::in8( PORT + 0 ) != 0xAE ) {
        support_serial_port = 0;
    }
    else {
        io_out8( PORT + 4, 0x0F );
        support_serial_port = 1;
    }


   return 0;
}

// 读操作
char read_serial(void) {
    if ( support_serial_port ) {
        while ( io_in8( PORT + 5 ) & 1 );
        return io_in8( PORT );
    }
    return '\0';
}
// 写操作
void write_serial(char ch) {
    if ( support_serial_port ) {
        while ( !( io_in8( PORT + 5 ) & 0x20 ) );
        io_out8( PORT, ch );
    }
    return;
}
void print(const char* str) {
    for (uint64_t i = 0 ; str[i] != '\0; ++i) {
        write_serial(str[i]);
    }
}
```
到目前为止, 一切正常的话, 我们就已经写好了串口, 你可以测试一下看看com端口是否有输出.  
在xmake.lua的qemu的flags中添加入`-serial chardev:com1 -chardev stdio,mux=on,id=com1`指令即可  
倘若成功，请查看下一个教程😋
# 自制OS教程#3 : 没鸟用全局段描述符表

- 作者提示 : 这玩意没啥用我就没详细看, 可能需要勘误

遇事不决, 先走一遍文档, intel手册是我们写OS最大的助手  
intel官网提供了下载地址[intel docx](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)  
先把手册搞到手，手中才算是有了工具，我们才能进一步开发  
全局描述符表在保护模式中以作为内存管理中的分段(Segmenting)的工具, 但是分段使用极其复杂, 实用性远不如分页(Paging), 以至于长模式直接强制使用了分页而非分段来进行内存管理, 分段机制也在长模式中被废除了  
既然全局描述符表在长模式已经没什么用了, 所以它仅起到一个类似占位符的作用, 目前来看, 只有syscall/sysret指令要利用到全局描述符表, 其中代码段, 数据段的顺序排列要遵循ABI规定  
我们只需要5个段描述符和一个系统段描述符(16byte, 又叫做任务状态段)  
全局描述符表的组成很简单, 就是由8byte大小的段描述符(Segment Descriptor)组成, 它最大可以有8192个段描述符, 段描述符结构如下  

| 63 -- 56 | 55 | 54 | 53 | 52       | 51 -- 48 | 47 | 46 -- 45 | 44 | 43 | 42 | 41 | 40 | 39 -- 16 | 15 -- 0 |
|----------|----|----|----|----------|----------|----|----------|----|----|----|----|----|----------|---------|
| base     | G	| DB | L  | reserved | limit    | P  | DPL      | S  | E  | DC | RW | A  | base     | limit   |

~~没错就是这么神经病  ~~
osdev里面是这么讲的: In 64-bit mode, the Base and Limit values __are ignored__, each descriptor covers the __entire linear__ address space regardless of what they are set to.  
base, limit这两个屁用没有, 但是我们还是得按照约定设置  
第一个段描述符是空的, 啥也不要弄  
第二个段描述符是内核代码段, base=0, limit=0xFFFFF, RW, E, S, P, G, L设置为1, DPL设置为0  
第三个段描述符是内核数据段, base=0, limit=0xFFFFF, RW, S, P, G, DB设置为1, DPL设置为0  
第四个段描述符是用户代码段, base=0, limit=0xFFFFF, RW, E, S, P, G, L设置为1, DPL设置为3  
第五个段描述符是用户数据段, base=0, limit=0xFFFFF, RW, S, P, G, DB设置为1, DPL设置为3  
这些位是干什么用的可以看intel手册`CHAPTER 3 PROTECTED-MODE MEMORY MANAGEMENT`.  

长模式下系统状态段结构如下
| 127 -- 96 | 95 -- 56 | 55 | 54 | 53 | 52       | 51 -- 48 | 47 | 46 -- 45 | 44 | 43 -- 40 | 39 -- 16 | 15 -- 0 |
|-----------|----------|----|----|----|----------|----------|----|----------|----|----------|----------|---------|
| reserved  | base     | G  | DB | L  | reserved | limit    | P  | DPL      | S  | type     | base     | limit   |

这个段就是给TSS准备的  
长模式下type(Type of system segment)可以填写如下数据:  
0x2: LDT  
0x9: 64-bit TSS (Available)  
0xB: 64-bit TSS (Busy)  

在第六个段描述符之前, 我们还得先写一下TSS(Task State Segment)结构
```c
typedef struct {
    uint32_t reserved1;
    uint64_t rsp[ 3 ];
    uint64_t reserved2;
    uint64_t ist[ 7 ];
    uint64_t reserved3;
    uint32_t io_map_base_address;
} TaskStateSegment;
```
你别看他叫任务状态段, 长模式下我们只用它来存储任务的中断内核栈地址(一般用rsp[0]来存).  
等我们到多核的时候会来回顾一下, 现在我们只需要一个.  
第六个段描述符是系统段描述符, 系统段描述符要占用两个段描述符的位置, base=你tss结构的地址, limit=__sizeof(TaskStateSegment)-1__, P设置为1, type设置为9  
在这些段描述符设置好后, 我们要做的就是写入GDT register(GDTR), 挂载GDT.  

GDTR结构体如下所示
```c
typedef struct {
    uint16_t size;   // sizeof(SegmentDescriptor) * 段描述符数量 - 1
    uint64_t offset; // 填入你的GDT地址
} GDTR;
```
然后, 用这样一条指令挂载GDT, 并设置段寄存器
```c
__asm__ __volatile__( "lgdt %0" ::"m"( gdtr ) : );
__asm__ __volatile__(
    "movq %%rax, %%ds \n\t"
    "movq %%rax, %%es \n\t"
    "movq %%rax, %%fs \n\t"
    "movq %%rax, %%gs \n\t"
    "movq %%rax, %%ss \n\t"
    "pushq %[SELECTOR_CODE64] \n\t"
    "leaq .next(%%rip),%%rax \n\t"
    "pushq %%rax \n\t"
    "lretq \n\r"
    ".next: \n\t" : : [SELECTOR_CODE64] "i"( SELECTOR_CODE64_KERNEL ),
                      [SELECTOR_DATA64] "a"( SELECTOR_DATA64_KERNEL ) : );
```
哦忘记告诉各位, SELECTOR_CODE64_KERNEL就是你段描述符在GDTR中的偏移量, 比如我内核代码段在GDT的偏移量就是0x8, 内核数据段在GDT的偏移量就是0x10<br>
还有一件事, 挂载tss
```c
 __asm__ __volatile__( "ltr %%ax" ::"a"( SELECTOR_TSS ) : "memory" );
```
SELECTOR_TSS的值为0x28, 也就是前文所提到的开头在GDT中的偏移量
做完这些, 我们GDT就算是初始化完成了.

# 自制OS教程#4 : 中断描述符表!中断初步
在intel白皮书的`CHAPTER 6 INTERRUPT AND EXCEPTION HANDLING`这一节中, 对中断进行了详细的介绍, 有兴趣的可以读一读(强烈建议先读文档再写, 我们写操作系统就是为了了解底层原理, 什么?你问我为什么那么爱调库?孩子你先好好去看看操作系统考级的大纲吧)  
中断描述符表(Interrupt Descriptor Table, IDT), 用于告知CPU中断服务处理程序(ISR)的位置, 一个IDT由256个门描述符描述符(Gate Descriptor)组成.  
门的类型共有三种, 中断门, 陷阱门, 任务门, 长模式里面任务门不存在, 只有前面两种门  
IDT其实根前面的GDT很像, 所以有关IDT我们就简单带过.  
门描述符结构如下
| 127 -- 96 | 95 -- 48 | 47 | 46 -- 45 | 44 | 43 -- 40  | 39 -- 35 | 34 -- 32 | 31     --     16 | 15 -- 0 |
|-----------|----------|----|----------|----|-----------|----------|----------|------------------|---------|
| reserved  | offset   | P  | DPL      | 0  | gate type | reserved | IST      | segment selector | offset  |

Selector指向GDT中的对应代码段(kernel, user), 这就是为什么我要先写GDT再写IDT  
Gate Type有两种值  
0b1110 or 0xE: 64-bit中断门  
0b1111 or 0xF: 64-bit陷阱门  
Offset指向你的中断处理服务函数的地址  
IST一般为0， 如果设置表示中断堆栈表在TSS中的偏移量  
P位一般都要设置为1  
先来看看我们要中断要保存什么: 段寄存器(ss, cs...), 通用寄存器(rax...), 栈指针(rsp), 程序计数器(RIP),程序状态字(PSW => rflags), 异常中断还有错误码(error code)

```c
typedef struct {
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t vector;
    uint64_t error_code;
    void *rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} InterruptFrame;
```

先把中断服务处理函数给写了吧, 但是我们要考虑哪些门是陷阱, 哪些是中断, 陷阱要考虑是否有错误码  
intel手册上面有详尽的介绍, 自己看

```asm

.macro TRAP_ENTRY_ERRORCODE n         
.GLOBAL interrupt_handler\n
interrupt_handler\n:
    CLI
    NOP
    PUSH \n
    JMP save_all_registers
.endm

.macro TRAP_ENTRY_NOERRORCODE n         
.GLOBAL interrupt_handler\n
interrupt_handler\n:
    CLI
    PUSH 0
    PUSH \n
    JMP save_all_registers
.endm

.macro INTERRUPT_ENTRY n                
.GLOBAL interrupt_handler\n
.section .text
interrupt_handler\n:
    CLI
    PUSH 0
    PUSH \n
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
    ADD RSP, 16
    IRETQ ; 不要忘了Q


TRAP_ENTRY_NOERRORCODE 0x00
TRAP_ENTRY_NOERRORCODE 0x01
TRAP_ENTRY_NOERRORCODE 0x02
TRAP_ENTRY_NOERRORCODE 0x03
TRAP_ENTRY_NOERRORCODE 0x04
TRAP_ENTRY_NOERRORCODE 0x05
TRAP_ENTRY_NOERRORCODE 0x06
; 这些都是处理头, 省略

```
do_IRQ函数是我们的总处理函数
```c
InterruptFrame* do_IRQ(InterruptFrame* frane) {
// handle可自由发挥, 但是对于exception要阻塞
}

```
然后, 注册中断处理函数, 一般来说0x00 - 0x1f都是trap, 其他都是interrupt  
也是宏魔法, 写的很长, 但是没有什么好解决办法  
C++26的反射似乎可以减少工作量, 但是目前没一个支持  
做完这些, 挂载IDT基本与GDT一致.  
只不过只需要一条**lidt**的指令就行了  
# 自制OS教程#5 : 页内存管理, limine崭露头角

# 自制OS教程#6 : 内核堆?你认真的?--Slab内存池
# 自制OS教程#7 : 分页--万恶之源, 萌新杀手
网传分页都是极其抽象且难以理解的, "萌新杀手", 今天我就带你玩转分页!  
分页是x86体系管理内存的重要功能, 强大之处多到难以表述, 分页在intel手册也是有详尽介绍, 就在`CHAPTER 4 PAGING`中. ~~就在分段后面, intel何意味~~  
分页组成很简单: 虚拟地址(virtual address), 页表(page table), MMU(Memory Manage Unit, 内存管理单元), 缺页处理(page fault handle)  
虚拟地址: 每级页表偏移量所构成的数字  
页表: 特殊的数据结构  
MMU: 对虚拟地址进行翻译, 将虚拟页面翻译成所对应的物理页框  
缺页处理: 进行写时复制, 利用页面置换算法挑选合适页进行置换, 或者panic  

我们先从页表入手吧  
一般, 最顶层页表的入口地址存在**cr3**寄存器里面  
cr3寄存器如下所示  
当cr4的pcid位为0时:
| 63 -- 32 | 11 -- 5  | 4 | 3 | 2 -- 0   |
|----------|----------|---|---|----------|
| address  | reserved |PCD|PWT| reserved |

为1时
| 63 -- 32 | 11 -- 0 |
|----------|---------|
| address  | PCID    |

然后, 从address中读出最顶层页表的入口地址, 这里以5级页表为例
一个五级页表是由512个四级页表入口组成的, 每个入口大小8byte, 但是每级的页目录项又有所不同
这个入口的基地址可能是下一级页表的入口地址, 也有可能是一个页的偏移地址(page offset).

- cr3可以看成pml5e -- 作者注
pml5t内部由512个入口项构成    
address指向下一级页表(pml4t)  

pml4e(page table level 4 entry, 下同)  
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 12 | 11  --  8 | 7 |     6     | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|---|-----------|---|---|---|---|---|---|
| XD | PK       | reserved | address   | available | 0 | available | A |PCD|PWT|U/S|R/W| P |

pml4t内部由512个入口项构成   
address指向下一级页表(pml3t)  

pml3e
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 12 | 11  --  8 | 7 |     6     | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|---|-----------|---|---|---|---|---|---|
| XD | PK       | reserved | address   | available | 0 | available | A |PCD|PWT|U/S|R/W| P |

pml3t内部由512个入口项构成  
PS(bit 7) 为0, address 指向下一级页表(pml2t)
PS(bit 7) 为1, address 指向1G大小的页  

pml2e  
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 12 | 11  --  8 |   7   |     6     | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|-------|-----------|---|---|---|---|---|---|
| XD | PK       | reserved | address   | available | PS(0) | available | A |PCD|PWT|U/S|R/W| P |

pge(1G huge page)
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 30 | 29 -- 13  | 12 | 11  --  9 | 8 |   7   | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|----|-----------|---|-------|---|---|---|---|---|---|---|
| XD | PK       | reserved | address   | reserved  |PAT | available | G | PS(1) | D | A |PCD|PWT|U/S|R/W| P |

pml2t内部由512个入口项构成  
PS(bit 7) 为0, address 指向下一级页表(pml1t)
PS(bit 7) 为1, address 指向2M大小的页
pml1e
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 12 | 11  --  8 |   7   |     6     | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|-------|-----------|---|---|---|---|---|---|
| XD | PK       | reserved | address   | available | PS(0) | available | A |PCD|PWT|U/S|R/W| P |

pge(2M huge page)
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 21 | 20 -- 13  | 12 | 11  --  9 | 8 |   7   | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|----|-----------|---|-------|---|---|---|---|---|---|---|
| XD | PK       | reserved | address   | reserved  |PAT | available | G | PS(1) | D | A |PCD|PWT|U/S|R/W| P |

pml1t内部由512个入口项构成  
pge(page entry, 4K standard page), address 对应一个4K页的入口
| 63 | 62 -- 52 | 51 -- M  | M-1 -- 12 | 11  --  9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|----|----------|----------|-----------|-----------|---|---|---|---|---|---|---|---|---|
| XD | PK       | reserved | address   | available | G |PAT| D | A |PCD|PWT|U/S|R/W| P |

可以看出, 页表是一种层层递进的特殊结构, 并且每个页表的大小都是4096字节

然后再看虚拟地址  

这是一个64位虚拟地址, 我们可以把它划分成这么几个部分  
| 63 ~ 57 | 56 ~ 48 | 47 ~ 39 | 38 ~ 30 | 29 ~ 21 | 20 ~ 12 | 11 ~ 0 |
|---------|---------|---------|---------|---------|---------|--------|
| 63 ~ 57 | 56 ~ 48 | 47 ~ 39 | 38 ~ 30 | 29 ~ 21 | 20 ~ 12 | 11 ~ 0 |
|available |available | pml5t offset   | pml4t offset   | pml3t offset   | pml2t offset   | pml1t offset  |

这个offset代表此虚拟地址在每个所在页表对应的虚拟页号(0 ~ 511)
除了页偏移是12位, 其他的都是9位偏移, 而每一级页表都是512=2^9个页目录  
他们之间存在什么联系么  
显然是有的  
比如我有一个虚拟地址0xffff800002311000, 指向一个4K page    
(0xffff800002311000 / 0x1000 = 0xffff800002311) % 512 = 273 => pml1t中的偏移  <=> 0xffff800002311000 >> 12 & 0x1ff  
(0xffff800002311 / 0x200 = 0x7fffc000011) % 512 = 17  => pml2t中的偏移        <=> 0xffff800002311000 >> 21 & 0x1ff  
(0x7fffc000011 / 0x200 = 0x3fffe0000) % 512 = 0 => pml3t中的偏移              <=> 0xffff800002311000 >> 30 & 0x1ff  
(0x3fffe0000 / 0x200 = 0x1ffff00 ) % 512 = 256 => pml4t中的偏移               <=> 0xffff800002311000 >> 39 & 0x1ff  
(0x1ffff00 / 0x200 = 0xffff) % 512 = 511 => pml5t中的偏移                     <=> 0xffff800002311000 >> 48 & 0x1ff  
(0xffff / 0x200 = 7f) % 512 = 127 => available中的偏移                        <=> 0xffff800002311000 >> 57 & 0x1ff  
(0x7f / 0x200 = 0) % 64 = 0 => available中的偏移                              <=> 0xffff800002311000 >> 64 & 0x1ff  
offset排列组合一下  
0 127 511 256 0 17 273  
反推  
0 * 2^64 + 127 * 2^57 + 511 * 2^48 + 256 * 2^39 + 0 * 2 ^ 30 + 17 * 2^21 + 273*2^12 = 0xffff800002311000  
我们把先前得到的offset转换为二进制全部填入之前的表格  
二进制表示为01111111 111111111 100000000 000000000 000010001 100010001 000000000000  
组合起来, 转换后得到的值就是0xffff800002311000, 与原来完全一致  
对于大页(huge page), 只需要理解为舍弃掉这一级页表以下的所有页表就行, 推导过程是一样的  

到目前为止, 我们已经讲解完了大部分的分页知识了  
MMU,则是起到物理地址与虚拟地址的转换的作用
每个页目录项内都有一个存储base address, 当ps位被设置或者当前页表为一级页表时, base address存储着对应物理地址
MMU 就是根据上面虚拟地址转换出虚拟页目录项, 在页表中进行逐级查找, 把得到的物理地址进行分解, 分解出对应的物理页框号与基地址, 大致与上面虚拟地址组合很像, 这设计到硬件我们就不过多深究了  
MMU中还有一个叫做转换检测缓冲区(Translation Lookaside Buffer, TLB, 又叫作快表)的固件, 它的存在有效提高了分页的效率, 做到快速转换
TLB的详细介绍在这里[osdev](https://wiki.osdev.org/TLB), 本文不多做叙述.  
刷新快表的时机要掌握, 在取消一个页的映射的时候或者映射一个新的页的时候就要刷新TLB  

最后一个就是缺页异常处理了, 这个章节内容我们放到后面再研究  

掌握了这些理论知识, 你就可以开始编写相对应的函数.
源码可以参考: [page_table.hpp](https://github.com/SegmentationFaultCD/QuantumNEC/blob/limine/include/kernel/memory/paging/page_table.hpp)以及[page_table.cpp](https://github.com/SegmentationFaultCD/QuantumNEC/blob/limine/source/kernel/memory/paging/page_table.cpp)  

或者, 使用我们的库: [libpaging](https://github.com/plos-clan/libpaging)

## 总结: 分页是实现操作系统的第一大难关, 但是只 要我们肯去认真研究, 查阅文档, 就不难掌握, 理解这些功能, 攻克难关
# 自制OS教程#8 : 拨开云雾, 打印输出
[os-terminal](https://github.com/plos-clan/libos-terminal) -- 优秀的终端库, 你要做的就是把他的release版本下载下来。  
把terminal.h和任意一个编译好的.a文件下载并链接进内核(ld指令怎么用我就不教了,自己查[ld](https://www.ibm.com/docs/en/aix/7.2.0?topic=l-ld-command))  
这就是为什么我要你直接写串口驱动而不是像别的教程一样先自己写printk--我们不需要自己写显存, 我们只用现成的库.  
但是我们总是要调试的, 如果出现错误, 我们就可以通过串口输出, 而不用费时费力思考怎么写显存  
怎么用就按照人家文档教的操作吧  
剩下的就是我们要自己写一个vsprintf/format, 得到格式化字符串, 然后调用ost提供的打印函数即可.  
整个流程我们就封装成printk, 或者你爱叫什么就叫什么.  
C++版本: format可以用我写的: [libformat](https://github.com/plos-clan/libformat), 按照教程操作  
C版本: vsprintf可以看CPOS的实现: [vsprintf]()  
这个, 应该不用教了吧, 封装打印，一条龙服务啊  
# 自制OS教程#9 : 高级中断处理
# 自制OS教程#10 : SIMD浮点支持
# 自制OS教程#11 : 多核!
# 自制OS教程#12 : 任务调度, 难上加难, 噩梦死锁的处理方法
# 自制OS教程#13 : 系统调用
# 自制OS教程#14 : 内核架构, 微内核设计与进程间通信
# 自制OS教程#15 : 初见端倪--文件系统与模块化
# 自制OS教程#17 : 大彻大悟--VFS
# 自制OS教程#18 : 设备驱动
# 自制OS教程#19 : 异常处理再一游--写时复制与页面置换算法
# 自制OS教程#20 : POSIX接口探秘
# 自制OS教程#21 : 动态链接器--古希腊掌管帮助减少体积的神