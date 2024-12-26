#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <libcxx/format.hpp>
namespace QuantumNEC::Kernel::x86_64 {
struct ControlRegisters {
    struct CR0 {
        uint64_t PE : 1;     // 如果PE=1，则保护模式启动，如果PE=0，则在实模式下运行
        uint64_t MP : 1;     // 监视协处理器
        uint64_t EM : 1;     // x87 FPU仿真
        uint64_t TS : 1;     // 任务已切换
        uint64_t ET : 1;     // 扩展类型
        uint64_t NE : 1;     // Numeric error
        uint64_t : 10;
        uint64_t WP : 1;     // 控制开启内存写保护，WP=0开启写保护，对只读页面尝试写入将触发异常，这一机制常常被用来实现写时复制功能
        uint64_t : 1;
        uint64_t AM : 1;     // 控制启用内存对齐自动检查
        uint64_t : 10;
        uint64_t NW : 1;     // 控制使用通写还是回写，用于维护内存数据一致性，NW=1关闭该功能，数据一致性即通过MESI协议
        uint64_t CD : 1;     // 控制Cache高速缓存功能，CD=1关闭该功能
        uint64_t PG : 1;     // 开启分页模式
        explicit CR0( void ) noexcept = default;
    };

    // CR1 保留
    // CPU在尝试访问它时将抛出#UD异常。

    struct CR2 {
        /*
         *   x86架构中的CR2寄存器是页故障线性地址寄存器，也称为页故障地址寄存器（Page Fault Linear Address Register）。当CPU检测到一个页故障（Page Fault）时，它会将故障发生时的线性地址存储在CR2寄存器中，这个地址指向导致页故障的页面。
         *   内核可以通过读取CR2寄存器来获取引起页故障的地址，然后对该地址所在的页面进行处理，比如进行页面的分配或者进行页面的交换。CR2寄存器的值可以在中断或异常处理例程中读取。
         *   在保护模式下，访问CR2寄存器需要使用特殊的指令，比如MOV CR2, EAX将EAX寄存器的值写入CR2寄存器，或MOV EAX, CR2将CR2寄存器的值读取到EAX寄存器中。在x86_64架构下，CR2寄存器的大小为64位。
         */

        uint64_t PFLA;
        explicit CR2( void ) noexcept = default;
    };
    struct CR3 {
        /*
         *   cr3寄存器和MMU密切相关，保存了当前进程所使用的虚拟地址空间的页目录地址，可以说是整个虚拟地址翻译中的顶级指挥棒，在进程空间切换的时候，CR3也将同步切换。
         *   cr3寄存器的高20位用于保存页目录地址，0-11位记录标记位，所以页目录地址必须是4KB的整数倍。
         */

        uint64_t : 3;
        uint64_t PWT : 1;
        uint64_t PCD : 1;
        uint64_t : 7;
        uint64_t page_directory_base : 52;
        explicit CR3( void ) noexcept = default;
    };
    struct CR4 {
        /*
         *   用于控制CPU的特性和操作系统的行为。它的作用如下：
         *   控制分页机制：x86CR4寄存器的最重要作用是控制分页机制。在64位x86处理器中，x86CR4寄存器的第5位（PAE）控制是否启用物理地址扩展（Physical Address Extension，PAE）模式，第7位（PSE）控制是否启用页大小扩展（Page Size Extension，PSE）模式，第12位（PCIDE）控制是否启用页表缓存（Page Directory Cache，PDC）。
         *   启用SMEP和SMAP保护机制：x86CR4寄存器的第20和第21位控制启用Supervisor Mode Execution Protection（SMEP）和Supervisor Mode Access Prevention（SMAP）保护机制。
         *   启用虚拟化扩展：x86CR4寄存器的第13位控制是否启用虚拟化扩展（Virtualization Extension，VME）。
         *   其他：x86CR4寄存器还控制着其他一些特性，如Debugging Extensions（DE）、Machine Check Exception（MCE）和Page Attribute Table（PAT）等。
         *   PVI	Protected-mode Virtual Interrupts
         *   TSD	Time Stamp Disable
         *   DE	    Debugging Extensions
         *   PSE	Page Size Extension
         *   PAE	Physical Address Extension
         *   MCE	Machine Check Exception
         *   PGE	Page Global Enabled
         *   PCE	Performance-Monitoring Counter enable
         *   OSFXSR	Operating system support for FXSAVE and FXRSTOR instructions
         *   OSXMMEXCPT	Operating System Support for Unmasked SIMD Floating-Point Exceptions
         *   UMIP	User-Mode Instruction Prevention (if set, #GP on SGDT, SIDT, SLDT, SMSW, and STR instructions when CPL > 0)
         *   VMXE	Virtual Machine Extensions Enable
         *   SMXE	Safer Mode Extensions Enable
         *   FSGSBASE	Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE
         *   PCIDE	PCID Enable
         *   OSXSAVE	XSAVE and Processor Extended States Enable
         *   SMEP	Supervisor Mode Execution Protection Enable
         *   SMAP	Supervisor Mode Access Prevention Enable
         *   PKE	Protection Key Enable
         *   CET	Control-flow Enforcement Technology
         *   PKS	Enable Protection Keys for Supervisor-Mode Pages
         */

        uint64_t VME : 1;
        uint64_t PVI : 1;
        uint64_t TSD : 1;
        uint64_t DE : 1;
        uint64_t PSE : 1;
        uint64_t PAE : 1;
        uint64_t MCE : 1;
        uint64_t PGE : 1;
        uint64_t PCE : 1;
        uint64_t OSFXSR : 1;
        uint64_t OSXMMEXCPT : 1;
        uint64_t UMIP : 1;
        uint64_t : 1;
        uint64_t VMXE : 1;
        uint64_t SMXE : 1;
        uint64_t : 1;
        uint64_t FSGSBASE : 1;
        uint64_t PCIDE : 1;
        uint64_t OSXSAVE : 1;
        uint64_t : 1;
        uint64_t SMEP : 1;
        uint64_t SMAP : 1;
        uint64_t PKE : 1;
        uint64_t CET : 1;
        uint64_t PKS : 1;
        uint64_t : 39;
        explicit CR4( void ) noexcept = default;
    };
    // CR5 ~ CR7 保留，如果使用结果和 CR1 一样.
    struct CR8 {
        // 任务优先级寄存器
        uint64_t TPL : 4;
        uint64_t : 60;
        explicit CR8( void ) noexcept = default;
    };
    // CR9 ~ CR15 保留，如果使用结果和 CR1 一样.
};

struct _packed RFlags {
    uint64_t CF : 1;
    uint64_t MBS : 1;
    uint64_t PF : 1;
    uint64_t : 1;
    uint64_t AF : 1;
    uint64_t : 1;
    uint64_t ZF : 1;
    uint64_t SF : 1;
    uint64_t TF : 1;
    uint64_t IF : 1;
    uint64_t DF : 1;
    uint64_t OF : 1;
    uint64_t IOPL : 2;
    uint64_t NT : 1;
    uint64_t : 1;
    uint64_t RF : 1;
    uint64_t VM : 1;
    uint64_t AC : 1;
    uint64_t VIF : 1;
    uint64_t VIP : 1;
    uint64_t ID : 1;
    uint64_t : 42;
    explicit RFlags( void ) noexcept = default;
    operator uint64_t( ) {
        return *reinterpret_cast< uint64_t * >( this );
    }
};

struct ControlRegisterFrame {
    ControlRegisters::CR0 cr0;
    ControlRegisters::CR2 cr2;
    ControlRegisters::CR3 cr3;
    ControlRegisters::CR4 cr4;
    ControlRegisters::CR8 cr8;
    explicit ControlRegisterFrame( void ) noexcept {
        ASM( "movq %%cr0, %0\n\t" : "=r"( this->cr0 ) );
        ASM( "movq %%cr2, %0\n\t" : "=r"( this->cr2 ) );
        ASM( "movq %%cr3, %0\n\t" : "=r"( this->cr3 ) );
        ASM( "movq %%cr4, %0\n\t" : "=r"( this->cr4 ) );
        ASM( "movq %%cr8, %0\n\t" : "=r"( this->cr8 ) );
    }
    // CR1，CR5~CR7, CR9~CR15为保留，无法访问
};
struct _packed GeneralPurposeRegistersFrame {
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
    explicit GeneralPurposeRegistersFrame( void ) noexcept = default;
};
struct _packed SegmentRegisterFrame {
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;
    explicit SegmentRegisterFrame( void ) noexcept = default;
};
struct _packed RegisterFrame :
    SegmentRegisterFrame,
    GeneralPurposeRegistersFrame {
    explicit RegisterFrame( void ) noexcept = default;
};

constexpr auto IA32_APIC_BASE_MSR { 0x1B };
constexpr auto IA32_APIC_BASE_MSR_BSP { 1UL << 8U };     // 处理器是 BSP
constexpr auto IA32_APIC_BASE_MSR_ENABLE { 1UL << 11U };
constexpr auto IA32_APIC_BASE_MSR_BASE_ADDR_MSK { ~0xFFFUL };
constexpr auto IA32_EFER { 0xc0000080 };
constexpr auto IA32_STAR { 0xc0000081 };
constexpr auto IA32_LSTAR { 0xc0000082 };
constexpr auto IA32_FMASK { 0xc0000084 };
constexpr auto IA32_EFER_SCE { 1 };

constexpr auto IA32_KERNEL_GS_BASE { 0xf0000102 };
constexpr auto IA32_USER_GS_BASE { 0xc0000101 };
constexpr auto IA32_USER_FS_BASE { 0xc0000100 };
// TIME

constexpr auto HZ { 100 };
constexpr auto OSCILLATOR { 1193182 };
constexpr auto CLOCK_COUNTER { OSCILLATOR / HZ };
constexpr auto JIFFY { 1000 / HZ };

// 蜂鸣器

constexpr auto SPEAKER_REG { 0x61 };
constexpr auto BEEP_HZ { 440 };     // 声音为440HZ

// 下面是 CMOS 信息的寄存器索引

constexpr auto CMOS_ADDRESS { 0x70 };     // CMOS 地址寄存器
constexpr auto CMOS_DATA { 0x71 };        // CMOS 数据寄存器

/* ------------------------------------------------------------------------------------ *
 * GDT内部：
 * n << 3 意味着 n * 2 ^ 3 = n * 8
 * 0x00 (0 << 3)    空描述符，保留
 * 0x08 (1 << 3)    64位内核代码段
 * 0x10 (2 << 3)    64位内核数据段
 * 0x18 (3 << 3)    32位用户代码段
 * 0x20 (4 << 3)    32位用户数据段
 * 0x28 (5 << 3)    64位用户代码段
 * 0x30 (6 << 3)    64位用户数据段
 * 0x38 (7 << 3)    32位内核代码段
 * 0x40 (8 << 3)    32位内核数据段
 * 0x48 (9 << 3)    空描述符，保留
 * 0x50 (10 << 3)   64位任务状态段低地址
 * 0x58 (11 << 3)   64位任务状态段高地址
 * 0x60 ~ 0x2000    保留，扩展用的
 * ------------------------------------------------------------------------------------ */

constexpr auto AR_G_4K { 0x8000 };
constexpr auto AR_D_32 { 0x4000 };
constexpr auto AR_D_64 { 0x0000 };
constexpr auto AR_L { 0x0000 };
constexpr auto AR_L_64 { 0x2000 };
constexpr auto AR_AVL { 0x0000 };
constexpr auto AR_P { 0x0080 };
constexpr auto AR_DPL_0 { 0x0000 };
constexpr auto AR_DPL_1 { 0x0020 };
constexpr auto AR_DPL_2 { 0x0040 };
constexpr auto AR_DPL_3 { 0x0060 };
constexpr auto AR_S_CODE { 0x0010 };
constexpr auto AR_S_DATA { 0x0010 };
constexpr auto AR_S_SYS { 0x0000 };
constexpr auto AR_TYPE_CODE { 0x0008 }; /* 可执行,非一致,不可读 */
constexpr auto AR_TYPE_DATA { 0x0002 }; /* 不可执行,向上拓展,可写 */
constexpr auto AR_TYPE_TSS { 0x0009 };
constexpr auto AR_CODE32 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_CODE | AR_TYPE_CODE };
constexpr auto AR_CODE32_DPL3 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_3 | AR_S_CODE | AR_TYPE_CODE };
constexpr auto AR_DATA32 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_DATA | AR_TYPE_DATA };
constexpr auto AR_DATA32_DPL3 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_3 | AR_S_DATA | AR_TYPE_DATA };
constexpr auto AR_CODE64 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_0 | AR_S_CODE | AR_TYPE_CODE };
constexpr auto AR_CODE64_DPL3 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_3 | AR_S_CODE | AR_TYPE_CODE };
constexpr auto AR_DATA64 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_0 | AR_S_DATA | AR_TYPE_DATA };
constexpr auto AR_DATA64_DPL3 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_3 | AR_S_DATA | AR_TYPE_DATA };
constexpr auto TSS_D_0 { 0 };
constexpr auto AR_TSS64 { AR_G_4K | TSS_D_0 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_SYS | AR_TYPE_TSS };
constexpr auto AR_TSS32 { AR_G_4K | TSS_D_0 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_SYS | AR_TYPE_TSS };
constexpr auto RPL0 { 0x0 };
constexpr auto RPL1 { 0x1 };
constexpr auto RPL2 { 0x2 };
constexpr auto RPL3 { 0x3 };
constexpr auto TI_GDT { 0x0 };
constexpr auto TI_LDT { 0x4 };

#include "./selector.h"

/* RFLAGS */
constexpr auto RFLAGS_CF { ( 1 << 0 ) };
constexpr auto RFLAGS_PF { ( 1 << 2 ) };
constexpr auto RFLAGS_AF { ( 1 << 4 ) };
constexpr auto RFLAGS_ZF { ( 1 << 6 ) };
constexpr auto RFLAGS_SF { ( 1 << 7 ) };
constexpr auto RFLAGS_TF { ( 1 << 8 ) };
constexpr auto RFLAGS_IF { ( 1 << 9 ) };
constexpr auto RFLAGS_DF { ( 1 << 10 ) };
constexpr auto RFLAGS_OF { ( 1 << 11 ) };
constexpr auto RFLAGS_IOPL1 { ( 1 << 12 ) };
constexpr auto RFLAGS_IOPL2 { ( 1 << 13 ) };
constexpr auto RFLAGS_NT { ( 1 << 14 ) };
constexpr auto RFLAGS_RF { ( 1 << 16 ) };
constexpr auto RFLAGS_VM { ( 1 << 17 ) };
constexpr auto RFLAGS_AC { ( 1 << 18 ) };
constexpr auto RFLAGS_VIF { ( 1 << 19 ) };
constexpr auto RFLAGS_VIP { ( 1 << 20 ) };
constexpr auto RFLAGS_ID { ( 1 << 21 ) };
constexpr auto RFLAGS_MBS { 1ull << 1 };
constexpr auto RFLAGS_IF_1 { 1ull << 9 };
constexpr auto RFLAGS_IF_0 { 0ull << 9 };
constexpr auto RFLAGS_IOPL_0 { 0ull << 12 };
constexpr auto RFLAGS_IOPL_3 { 3ull << 12 };

/* ------------------------------------------------------------------------------------
 * IDT内部：
 * 0 ~ 20   异常入口
 * 21 ~ 26  保留
 * 27 ~ 29  异常入口
 * 30 ~ 256 中断入口
    - 30 ~ 31 无用处
    - 32 时间中断(apic以后弃用)
    - 33 键盘中断
    - 34 级联(两个芯片内部使用。不会发起)/HPET timer 0/8254 counter 0
    - 35 串口COM2&4对应的入口
    - 36 串口COM1&3对应的入口
    - 37 并口LPT2对应的入口
    - 38 软盘对应的入口
    - 39 并口LPT1对应的入口 [通常是“伪”中断（不可靠）]
    - 40 CMOS实时时钟/HPET timer 1
    - 41 自由外设/legacy SCSI/网卡
    - 42 自由外设/SCSI/网卡
    - 43 HPET timer 2/自由外设/SCSI/网卡
    - 44 HPET timer 3/PS2鼠标接口
    - 45 FPU/协处理器/间处理器
    - 46 主ATA硬盘
    - 47 从ATA硬盘
    - 48 PIRQA
    - 49 PIRQB
    - 50 PIRQC
    - 51 PIRQD
    - 52 PIRQE
    - 53 PIRQF
    - 54 PIRQG
    - 55 PIRQH
    - 128 系统调用
    - 150 ~ 200	Local APIC
    - 150	CMCI
    - 151	Timer
    - 152	Thermal Monitor
    - 153	Performance Counter
    - 154	LINT0
    - 155	LINT1
    - 156	Error
    - 200 ~ 255 SMP

 * ------------------------------------------------------------------------------------ */

// -
constexpr auto IDT_DESC_P { 1 };
constexpr auto IDT_DESC_DPL0 { 0 };
constexpr auto IDT_DESC_DPL3 { 3 };
constexpr auto IDT_DESC_64_TYPE { 0xF };     // 64 位的门
constexpr auto IDT_DESC_32_TYPE { 0xE };     // 32 位的门
constexpr auto IDT_DESC_16_TYPE { 0x6 };     // 16 位的门

constexpr auto NR_CPUS { 256 };
constexpr auto INTERRUPT_DESCRIPTOR_TABLE_COUNT { 1 };
constexpr auto INTERRUPT_DESCRIPTOR_COUNT { 256 };
constexpr auto GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT { NR_CPUS };
constexpr auto SEGMENT_DESCRIPTOR_COUNT { 32 };
constexpr auto TASK_STATE_SEGMENT_DESCRIPTOR_COUNT { NR_CPUS };

/** @brief -------------最大CORE数量 **/
constexpr auto INTERRUPT_ENTRY_COUNT { 256 };
constexpr auto IDT_ENTRY_IRQ_0 { 0x20 };
constexpr auto IRQ_CLOCK { IDT_ENTRY_IRQ_0 };
constexpr auto IRQ_KEYBOARD { IDT_ENTRY_IRQ_0 + 0x01 };
constexpr auto IRQ_CASCADE_TIMER { IDT_ENTRY_IRQ_0 + 0x02 };
constexpr auto IRQ_CMOS_RTC { IDT_ENTRY_IRQ_0 + 0x08 };
constexpr auto IRQ_APIC_LVT_LINT1 { IDT_ENTRY_IRQ_0 + 123 };
constexpr auto IRQ_APIC_LVT_LINT0 { IDT_ENTRY_IRQ_0 + 122 };
constexpr auto IRQ_APIC_LVT_ERROR { IDT_ENTRY_IRQ_0 + 124 };
constexpr auto IRQ_APIC_LVT_CMCI { IDT_ENTRY_IRQ_0 + 118 };
constexpr auto IRQ_APIC_LVT_TIMER { IDT_ENTRY_IRQ_0 + 119 };
constexpr auto IRQ_APIC_LVT_THERMAL_MONITOR { IDT_ENTRY_IRQ_0 + 120 };
constexpr auto IRQ_APIC_LVT_PERFORMANCE_COUNTER { IDT_ENTRY_IRQ_0 + 121 };
constexpr auto IRQ_SYSTEM_CALL { IDT_ENTRY_IRQ_0 + 0x60 };
constexpr auto IDT_IRQ_SMP_INTERRUPT_0 { 0xc8 };
constexpr auto IRQ_SMP_SWITCH_CPU { IDT_IRQ_SMP_INTERRUPT_0 };

// APIC
/*
    Base Address   MSR address  Name                        Attribute
    0xfee00000     none         保留                         N/A
    0xfee00010     none         保留                         N/A
    0xfee00020     0x802        Local APIC ID寄存器          RW
    0xfee00030     0x803        Local APIC Version寄存器     RO
    0xfee00040     none         保留                         N/A
    ~
    0xfee00070     none         保留                         N/A
    0xfee00080     0x808        任务优先级寄存器TPR            RW
    0xfee00090     none         优先级仲裁寄存器APR            RO
    0xfee000a0     0x80a        处理器优先级寄存器PPR          RO
    0xfee000b0     0x80b        EOI寄存器                    WO
    0xfee000c0     none         远程读取寄存器RRD             RO
    0xfee000d0     0x80d        逻辑目标寄存器LDR             RW
    0xfee000e0     none         目标格式寄存器DFR             RW
    0xfee000f0     0x80f        伪中断向量寄存器SVR           RW
    0xfee00100     0x810        ISR寄存器(31:0)             RO
    0xfee00110     0x811        ISR寄存器(63:32)            RO
    0xfee00120     0x812        ISR寄存器(95:64)            RO
    0xfee00130     0x813        ISR寄存器(127:96)           RO
    0xfee00140     0x814        ISR寄存器(159:128)          RO
    0xfee00150     0x815        ISR寄存器(191:160)          RO
    0xfee00160     0x816        ISR寄存器(223:192)          RO
    0xfee00170     0x817        ISR寄存器(255:224)          RO
    0xfee00180     0x818        TMR寄存器(31:0)             RO
    0xfee00190     0x819        TMR寄存器(63:32)            RO
    0xfee001a0     0x81a        TMR寄存器(95:64)            RO
    0xfee001b0     0x81b        TMR寄存器(127:96)           RO
    0xfee001c0     0x81c        TMR寄存器(159:128)          RO
    0xfee001d0     0x81d        TMR寄存器(191:160)          RO
    0xfee001e0     0x81e        TMR寄存器(223:192)          RO
    0xfee001f0     0x81f        TMR寄存器(255:224)          RO
    0xfee00200     0x820        IRR寄存器(31:0)             RO
    0xfee00210     0x821        IRR寄存器(63:32)            RO
    0xfee00220     0x822        IRR寄存器(95:64)            RO
    0xfee00230     0x823        IRR寄存器(127:96)           RO
    0xfee00240     0x824        IRR寄存器(159:128)          RO
    0xfee00250     0x825        IRR寄存器(191:160)          RO
    0xfee00260     0x826        IRR寄存器(223:192)          RO
    0xfee00270     0x827        IRR寄存器(255:224)          RO
    0xfee00280     0x828        ESR错误状态寄存器             RO
    0xfee00290     none         保留                        N/A
    ~
    0xfee002e0     none         保留                        N/A
    0xfee002f0     0x82f        LVT CMCI寄存器              RW
    0xfee00300     0x830        中断命令寄存器ICR (31：0)     RW
    0xfee00310     0x831        中断命令寄存器ICR (63：32)    RW
    0xfee00320     0x832        LVT定时器寄存器              RW
    0xfee00330     0x833        LVT温度传感器寄存器           RW
    0xfee00340     0x834        LVT性能监控计数器寄存器        RW
    0xfee00350     0x835        LVTLINT0寄存器              RW
    0xfee00360     0x836        LVTLINT1寄存器              RW
    0xfee00370     0x837        LVT错误寄存器                RW
    0xfee00380     0x838        初始计数寄存器（定时器专用）    RW
    0xfee00390     0x839        当前计数寄存器（定时器专用）    RW
    0xfee003a0     none         保留                        N/A
    ~
    0xfee003d0     none         保留                        N/A
    0xfee003e0     0x83e        分频配置寄存器（定时器专用）    N/A
    N/A            0x83f        SELF IPI寄存器              N/A
*/

//
constexpr auto LOCAL_BASE_APIC_ID { 0x0020 };
constexpr auto LOCAL_BASE_APIC_VERSION { 0x0030 };
constexpr auto LOCAL_BASE_APIC_TPR { 0x0080 };
constexpr auto LOCAL_BASE_APIC_APR { 0x0090 };
constexpr auto LOCAL_BASE_APIC_PPR { 0x00A0 };
constexpr auto LOCAL_BASE_APIC_EOI { 0x00B0 };
constexpr auto LOCAL_BASE_APIC_RRD { 0x00C0 };
constexpr auto LOCAL_BASE_APIC_LDR { 0x00D0 };
constexpr auto LOCAL_BASE_APIC_DFR { 0x00E0 };
constexpr auto LOCAL_BASE_APIC_SVR { 0x00F0 };
constexpr auto LOCAL_BASE_APIC_ENABLE { 0x00000100 };
constexpr auto LOCAL_BASE_APIC_ESR { 0x0280 };
constexpr auto LOCAL_BASE_APIC_LVT_CMCI { 0x02F0 };
constexpr auto LOCAL_BASE_APIC_ICRL0 { 0x0300 };
constexpr auto LOCAL_BASE_APIC_ICRL1 { 0x0310 };
constexpr auto LOCAL_BASE_APIC_LVT_TIMER { 0x0320 };
constexpr auto LOCAL_BASE_APIC_LVT_TS { 0x0330 };
constexpr auto LOCAL_BASE_APIC_LVT_PMC { 0x0340 };
constexpr auto LOCAL_BASE_APIC_LVT_LINT0 { 0x0350 };
constexpr auto LOCAL_BASE_APIC_LVT_LINT1 { 0x0360 };
constexpr auto LOCAL_BASE_APIC_LVT_ERROR { 0x0370 };
constexpr auto LOCAL_BASE_APIC_TICR { 0x0380 };
constexpr auto LOCAL_BASE_APIC_TCCR { 0x0390 };
constexpr auto LOCAL_BASE_APIC_FDCR { 0x03D0 };
constexpr auto LOCAL_BASE_APIC_TDCR { 0x03E0 };

constexpr auto LOCAL_APIC_MSR_SVR { 0x80f };
constexpr auto LOCAL_APIC_MSR_VERSION { 0x803 };
constexpr auto LOCAL_APIC_MSR_LVT_CMCI { 0x82f };
constexpr auto LOCAL_APIC_MSR_ICRL0 { 0x830 };
constexpr auto LOCAL_APIC_MSR_ICRL1 { 0x831 };
constexpr auto LOCAL_APIC_MSR_LVT_TIMER { 0x832 };
constexpr auto LOCAL_APIC_MSR_LVT_TS { 0x833 };
constexpr auto LOCAL_APIC_MSR_LVT_PMC { 0x834 };
constexpr auto LOCAL_APIC_MSR_LVT_LINT0 { 0x835 };
constexpr auto LOCAL_APIC_MSR_LVT_LINT1 { 0x836 };
constexpr auto LOCAL_APIC_MSR_LVT_ERROR { 0x837 };
constexpr auto LOCAL_APIC_MSR_ESR { 0x828 };
constexpr auto LOCAL_APIC_MSR_EOI { 0x80b };
constexpr auto LOCAL_APIC_MSR_ICRLO { 0x830 };
constexpr auto LOCAL_APIC_MSR_ICRLI { 0x831 };
constexpr auto LOCAL_APIC_MSR_TPR { 0x808 };
constexpr auto LOCAL_APIC_MSR_TICR { 0x838 };
constexpr auto LOCAL_APIC_MSR_TCCR { 0x839 };
constexpr auto LOCAL_APIC_MSR_TDCR { 0x83e };

constexpr auto APIC_INIT { 0x00000500 };
constexpr auto APIC_STARTUP { 0x00000600 };
constexpr auto APIC_DELIVS { 0x00001000 };
constexpr auto APIC_ASSERT { 0x00004000 };
constexpr auto APIC_DEASSERT { 0x00000000 };
constexpr auto APIC_LEVEL { 0x00008000 };
constexpr auto APIC_BCAST { 0x00080000 };
constexpr auto APIC_BUSY { 0x00001000 };
constexpr auto APIC_FIXED { 0x00000000 };
constexpr auto APIC_X1 { 0x0000000B };
constexpr auto APIC_PERIODIC { 0x00020000 };
constexpr auto APIC_PCINT { 0x0340 };
constexpr auto APIC_MASKED { 0x00010000 };
constexpr auto IOAPIC_REG_ID { 0x00 };
constexpr auto IOAPIC_REG_VER { 0x01 };
/* 重定向表从 REG_TABLE 开始，使用两个寄存器来配置每个中断。一对中的第一个（低电平）寄存器包含配置位。 第二个（高）寄存器包含一个位掩码，告诉哪个CPU 可以提供该中断。 */
constexpr auto IOAPIC_REG_TABLE { 0x10 };
constexpr auto INT_DISABLED { 0x00010000 };      // 中断已禁用
constexpr auto INT_LEVEL { 0x00008000 };         // 电平触发（与边沿-）
constexpr auto INT_ACTIVELOW { 0x00002000 };     // 低电平有效（与高电平相比）
constexpr auto INT_LOGICAL { 0x00000800 };       // 目标为 CPU ID（与 APIC ID 相对）

// delivery mode

constexpr auto APIC_ICR_IOAPIC_FIXED { 0 };
constexpr auto IOAPIC_ICR_LOWEST_PRIORITY { 1 };
constexpr auto APIC_ICR_IOAPIC_SMI { 2 };
constexpr auto APIC_ICR_IOAPIC_NMI { 4 };
constexpr auto APIC_ICR_IOAPIC_INIT { 5 };
constexpr auto ICR_START_UP { 6 };
constexpr auto IOAPIC_EXTINT { 7 };

// timer mode

constexpr auto APIC_LVT_TIMER_ONE_SHOT { 0 };
constexpr auto APIC_LVT_TIMER_PERIODIC { 1 };
constexpr auto APIC_LVT_TIMER_TSC_DEADLINE { 2 };

// mask

constexpr auto APIC_ICR_IOAPIC_MASKED { 1 };
constexpr auto APIC_ICR_IOAPIC_UNMASKED { 0 };

// trigger mode

constexpr auto APIC_ICR_IOAPIC_EDGE { 0 };
constexpr auto APIC_ICR_IOAPIC_LEVEL { 1 };

// delivery status

constexpr auto APIC_ICR_IOAPIC_IDLE { 0 };
constexpr auto APIC_ICR_IOAPIC_SEND_PENDING { 1 };

// destination shorthand

constexpr auto ICR_NO_SHORTHAND { 0 };
constexpr auto ICR_SELF { 1 };
constexpr auto ICR_ALL_INCLUDE_SELF { 2 };
constexpr auto ICR_ALL_EXCLUDE_SELF { 3 };

// destination mode

constexpr auto ICR_IOAPIC_DELV_PHYSICAL { 0 };
constexpr auto ICR_IOAPIC_DELV_LOGIC { 1 };

// level

constexpr auto ICR_LEVEL_DE_ASSERT { 0 };
constexpr auto ICR_LEVLE_ASSERT { 1 };

// remote irr

constexpr auto APIC_IOAPIC_IRR_RESET { 0 };
constexpr auto APIC_IOAPIC_IRR_ACCEPT { 1 };

// pin polarity

constexpr auto APIC_IOAPIC_POLARITY_HIGH { 0 };
constexpr auto APIC_IOAPIC_POLARITY_LOW { 1 };

constexpr auto PIC_M_CTRL { 0x20 }; /* 主 PIC 的 IO 基址 */
constexpr auto PIC_S_CTRL { 0xA0 }; /* 从 PIC 的 IO 基址 */
constexpr auto PIC_M_COMMAND { PIC_M_CTRL };
constexpr auto PIC_M_DATA { ( PIC_M_CTRL + 1 ) }; /* 主数据 */
constexpr auto PIC_S_COMMAND { PIC_S_CTRL };
constexpr auto PIC_S_DATA { ( PIC_S_CTRL + 1 ) }; /* 从数据 */
constexpr auto PIC_EOI { 0x20 };                  /* 中断结束 */
constexpr auto ICW1 { 0x11 };                     /* 中断控制命令字PIC用于初始化 */
constexpr auto ICW1_ICW4 { 0x01 };                /* 表示ICW4将出现 */
constexpr auto ICW1_SINGLE { 0x02 };              /* 单（级联）模式 */
constexpr auto ICW1_INTERVAL4 { 0x04 };           /* 呼叫地址间隔4（8） */
constexpr auto ICW1_LEVEL { 0x08 };               /* 电平触发（边缘）模式 */
constexpr auto ICW1_INIT { 0x10 };                /* 初始化-必需！*/
constexpr auto ICW4_8086 { 0x01 };                /* 8086/88（MCS-80/85）模式 */
constexpr auto ICW4_AUTO { 0x02 };                /* 自动（正常）EOI */
constexpr auto ICW4_BUF_SLAVE { 0x08 };           /* 缓冲模式/从片 */
constexpr auto ICW4_BUF_MASTER { 0x0C };          /* 缓冲模式/主片 */
constexpr auto ICW4_SFNM { 0x10 };                /* 特殊完全嵌套（非） */
constexpr auto PIC_READ_IRR { 0x0a };             /* OCW3 irq准备好下一次CMD读取 */
constexpr auto PIC_READ_ISR { 0x0b };             /* OCW3 irq服务下一次CMD读取 */

// 8254 PIT

constexpr auto IRQ0_FREQUENCY { 100 };
constexpr auto INPUT_FREQUENCY { 1193180 };
constexpr auto COUNTER0_VALUE { INPUT_FREQUENCY / IRQ0_FREQUENCY };
constexpr auto CONTRER0_PORT { 0x40 };
constexpr auto COUNTER0_NO { 0 };
constexpr auto COUNTER_MODE { 2 };
constexpr auto READ_WRITE_LATCH { 3 };
constexpr auto PIT_CONTROL_PORT { 0x43 };

}     // namespace QuantumNEC::Kernel::x86_64
namespace QuantumNEC::Kernel {
inline BootConfig __config { };

}
namespace std {
template <>
struct formatter< QuantumNEC::Kernel::x86_64::RFlags > {
    auto format( IN const QuantumNEC::Kernel::x86_64::RFlags &flags ) -> char * {
        auto val = *( (uint64_t *)&flags );
        return formatter< uint64_t > { }.format( val );
    }
};

}     // namespace std
