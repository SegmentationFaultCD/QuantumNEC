#pragma once
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC struct ControlRegisterFrame
    {
        Lib::Types::uint64_t cr0;
        Lib::Types::uint64_t cr2;
        Lib::Types::uint64_t cr3;
        Lib::Types::uint64_t cr4;
        Lib::Types::uint64_t cr8;
        explicit ControlRegisterFrame( VOID ) noexcept;
        ~ControlRegisterFrame( VOID ) noexcept;
        // CR1，CR5~CR7, CR9~CR15为保留，无法访问
    } _packed;
    PUBLIC struct GeneralPurposeRegistersFrame
    {
        Lib::Types::uint64_t rax;
        Lib::Types::uint64_t rbx;
        Lib::Types::uint64_t rcx;
        Lib::Types::uint64_t rdx;
        Lib::Types::uint64_t rbp;
        Lib::Types::uint64_t rsi;
        Lib::Types::uint64_t rdi;
        Lib::Types::uint64_t r8;
        Lib::Types::uint64_t r9;
        Lib::Types::uint64_t r10;
        Lib::Types::uint64_t r11;
        Lib::Types::uint64_t r12;
        Lib::Types::uint64_t r13;
        Lib::Types::uint64_t r14;
        Lib::Types::uint64_t r15;
    } _packed;
    PUBLIC struct SegmentRegisterFrame
    {
        Lib::Types::uint64_t ds;
        Lib::Types::uint64_t es;
        Lib::Types::uint64_t fs;
        Lib::Types::uint64_t gs;
    } _packed;
    PUBLIC struct RegisterFrame :
        SegmentRegisterFrame,
        GeneralPurposeRegistersFrame
    {
    } _packed;
    struct ControlRegisters
    {
        struct CR0
        {
            Lib::Types::uint64_t PE : 1;     // 如果PE=1，则保护模式启动，如果PE=0，则在实模式下运行
            Lib::Types::uint64_t MP : 1;     // 监视协处理器
            Lib::Types::uint64_t EM : 1;     // x87 FPU仿真
            Lib::Types::uint64_t TS : 1;     // 任务已切换
            Lib::Types::uint64_t ET : 1;     // 扩展类型
            Lib::Types::uint64_t NE : 1;     // Numeric error
            Lib::Types::uint64_t : 10;
            Lib::Types::uint64_t WP : 1;     // 控制开启内存写保护，WP=0开启写保护，对只读页面尝试写入将触发异常，这一机制常常被用来实现写时复制功能
            Lib::Types::uint64_t : 1;
            Lib::Types::uint64_t AM : 1;     // 控制启用内存对齐自动检查
            Lib::Types::uint64_t : 10;
            Lib::Types::uint64_t NW : 1;     // 控制使用通写还是回写，用于维护内存数据一致性，NW=1关闭该功能，数据一致性即通过MESI协议
            Lib::Types::uint64_t CD : 1;     // 控制Cache高速缓存功能，CD=1关闭该功能
            Lib::Types::uint64_t PG : 1;     // 开启分页模式
        };

        // CR1 保留
        // CPU在尝试访问它时将抛出#UD异常。

        struct CR2
        {
            /*
             *   x86架构中的CR2寄存器是页故障线性地址寄存器，也称为页故障地址寄存器（Page Fault Linear Address Register）。当CPU检测到一个页故障（Page Fault）时，它会将故障发生时的线性地址存储在CR2寄存器中，这个地址指向导致页故障的页面。
             *   内核可以通过读取CR2寄存器来获取引起页故障的地址，然后对该地址所在的页面进行处理，比如进行页面的分配或者进行页面的交换。CR2寄存器的值可以在中断或异常处理例程中读取。
             *   在保护模式下，访问CR2寄存器需要使用特殊的指令，比如MOV CR2, EAX将EAX寄存器的值写入CR2寄存器，或MOV EAX, CR2将CR2寄存器的值读取到EAX寄存器中。在x86_64架构下，CR2寄存器的大小为64位。
             */

            Lib::Types::uint64_t PFLA;
        };
        struct CR3
        {
            /*
             *   cr3寄存器和MMU密切相关，保存了当前进程所使用的虚拟地址空间的页目录地址，可以说是整个虚拟地址翻译中的顶级指挥棒，在进程空间切换的时候，CR3也将同步切换。
             *   cr3寄存器的高20位用于保存页目录地址，0-11位记录标记位，所以页目录地址必须是4KB的整数倍。
             */

            Lib::Types::uint64_t : 3;
            Lib::Types::uint64_t PWT : 1;
            Lib::Types::uint64_t PCD : 1;
            Lib::Types::uint64_t : 7;
            Lib::Types::uint64_t page_directory_base : 52;
        };
        struct CR4
        {
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

            Lib::Types::uint64_t VME : 1;
            Lib::Types::uint64_t PVI : 1;
            Lib::Types::uint64_t TSD : 1;
            Lib::Types::uint64_t DE : 1;
            Lib::Types::uint64_t PSE : 1;
            Lib::Types::uint64_t PAE : 1;
            Lib::Types::uint64_t MCE : 1;
            Lib::Types::uint64_t PGE : 1;
            Lib::Types::uint64_t PCE : 1;
            Lib::Types::uint64_t OSFXSR : 1;
            Lib::Types::uint64_t OSXMMEXCPT : 1;
            Lib::Types::uint64_t UMIP : 1;
            Lib::Types::uint64_t : 1;
            Lib::Types::uint64_t VMXE : 1;
            Lib::Types::uint64_t SMXE : 1;
            Lib::Types::uint64_t : 1;
            Lib::Types::uint64_t FSGSBASE : 1;
            Lib::Types::uint64_t PCIDE : 1;
            Lib::Types::uint64_t OSXSAVE : 1;
            Lib::Types::uint64_t : 1;
            Lib::Types::uint64_t SMEP : 1;
            Lib::Types::uint64_t SMAP : 1;
            Lib::Types::uint64_t PKE : 1;
            Lib::Types::uint64_t CET : 1;
            Lib::Types::uint64_t PKS : 1;
            Lib::Types::uint64_t : 39;
        };
        // CR5 ~ CR7 保留，如果使用结果和 CR1 一样.
        struct CR8
        {
            // 任务优先级寄存器
            Lib::Types::uint64_t TPL : 4;
            Lib::Types::uint64_t : 60;
        };
        // CR9 ~ CR15 保留，如果使用结果和 CR1 一样.
    };

    PUBLIC constexpr CONST auto IA32_APIC_BASE_MSR { 0x1B };
    PUBLIC constexpr CONST auto IA32_APIC_BASE_MSR_BSP { 1UL << 8U };     // 处理器是 BSP
    PUBLIC constexpr CONST auto IA32_APIC_BASE_MSR_ENABLE { 1UL << 11U };
    PUBLIC constexpr CONST auto IA32_APIC_BASE_MSR_BASE_ADDR_MSK { ~0xFFFUL };
    PUBLIC constexpr CONST auto IA32_EFER { 0xc0000080 };
    PUBLIC constexpr CONST auto IA32_STAR { 0xc0000081 };
    PUBLIC constexpr CONST auto IA32_LSTAR { 0xc0000082 };
    PUBLIC constexpr CONST auto IA32_FMASK { 0xc0000084 };
    PUBLIC constexpr CONST auto IA32_EFER_SCE { 1 };

    // TIME

    PUBLIC constexpr CONST auto HZ { 100 };
    PUBLIC constexpr CONST auto OSCILLATOR { 1193182 };
    PUBLIC constexpr CONST auto CLOCK_COUNTER { OSCILLATOR / HZ };
    PUBLIC constexpr CONST auto JIFFY { 1000 / HZ };

    // 蜂鸣器

    PUBLIC constexpr CONST auto SPEAKER_REG { 0x61 };
    PUBLIC constexpr CONST auto BEEP_HZ { 440 };     // 声音为440HZ

    // 下面是 CMOS 信息的寄存器索引

    PUBLIC constexpr CONST auto CMOS_ADDRESS { 0x70 };     // CMOS 地址寄存器
    PUBLIC constexpr CONST auto CMOS_DATA { 0x71 };        // CMOS 数据寄存器
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

    PUBLIC constexpr const auto AR_G_4K { 0x8000 };
    PUBLIC constexpr const auto AR_D_32 { 0x4000 };
    PUBLIC constexpr const auto AR_D_64 { 0x0000 };
    PUBLIC constexpr const auto AR_L { 0x0000 };
    PUBLIC constexpr const auto AR_L_64 { 0x2000 };
    PUBLIC constexpr const auto AR_AVL { 0x0000 };
    PUBLIC constexpr const auto AR_P { 0x0080 };
    PUBLIC constexpr const auto AR_DPL_0 { 0x0000 };
    PUBLIC constexpr const auto AR_DPL_1 { 0x0020 };
    PUBLIC constexpr const auto AR_DPL_2 { 0x0040 };
    PUBLIC constexpr const auto AR_DPL_3 { 0x0060 };
    PUBLIC constexpr const auto AR_S_CODE { 0x0010 };
    PUBLIC constexpr const auto AR_S_DATA { 0x0010 };
    PUBLIC constexpr const auto AR_S_SYS { 0x0000 };
    PUBLIC constexpr const auto AR_TYPE_CODE { 0x0008 }; /* 可执行,非一致,不可读 */
    PUBLIC constexpr const auto AR_TYPE_DATA { 0x0002 }; /* 不可执行,向上拓展,可写 */
    PUBLIC constexpr const auto AR_TYPE_TSS { 0x0009 };
    PUBLIC constexpr const auto AR_CODE32 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_CODE | AR_TYPE_CODE };
    PUBLIC constexpr const auto AR_CODE32_DPL3 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_3 | AR_S_CODE | AR_TYPE_CODE };
    PUBLIC constexpr const auto AR_DATA32 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_DATA | AR_TYPE_DATA };
    PUBLIC constexpr const auto AR_DATA32_DPL3 { AR_G_4K | AR_D_32 | AR_L | AR_AVL | AR_P | AR_DPL_3 | AR_S_DATA | AR_TYPE_DATA };
    PUBLIC constexpr const auto AR_CODE64 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_0 | AR_S_CODE | AR_TYPE_CODE };
    PUBLIC constexpr const auto AR_CODE64_DPL3 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_3 | AR_S_CODE | AR_TYPE_CODE };
    PUBLIC constexpr const auto AR_DATA64 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_0 | AR_S_DATA | AR_TYPE_DATA };
    PUBLIC constexpr const auto AR_DATA64_DPL3 { AR_G_4K | AR_D_64 | AR_L_64 | AR_AVL | AR_P | AR_DPL_3 | AR_S_DATA | AR_TYPE_DATA };
    PUBLIC constexpr const auto TSS_D_0 { 0 };
    PUBLIC constexpr const auto AR_TSS64 { AR_G_4K | TSS_D_0 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_SYS | AR_TYPE_TSS };
    PUBLIC constexpr const auto AR_TSS32 { AR_G_4K | TSS_D_0 | AR_L | AR_AVL | AR_P | AR_DPL_0 | AR_S_SYS | AR_TYPE_TSS };
    PUBLIC constexpr const auto RPL0 { 0x0 };
    PUBLIC constexpr const auto RPL1 { 0x1 };
    PUBLIC constexpr const auto RPL2 { 0x2 };
    PUBLIC constexpr const auto RPL3 { 0x3 };
    PUBLIC constexpr const auto TI_GDT { 0x0 };
    PUBLIC constexpr const auto TILDT { 0x4 };
    PUBLIC constexpr const auto SELECTOR_CODE64_KERNEL { ( 1ull << 3 ) | TI_GDT | RPL0 }; /* 64位内核代码段 */
    PUBLIC constexpr const auto SELECTOR_DATA64_KERNEL { ( 2ull << 3 ) | TI_GDT | RPL0 }; /* 64位内核数据段 */
    PUBLIC constexpr const auto SELECTOR_CODE64_USER { ( 5ull << 3 ) | TI_GDT | RPL3 };   /* 64位用户代码段 */
    PUBLIC constexpr const auto SELECTOR_DATA64_USER { ( 6ull << 3 ) | TI_GDT | RPL3 };   /* 64位用户数据段 */
    PUBLIC constexpr const auto SELECTOR_TSS { ( 10ull << 3 ) | TI_GDT | RPL0 };          /* TSS段 */

    /* RFLAGS */
    PUBLIC constexpr const auto RFLAGS_IF { ( 1ull << 9 ) };
    PUBLIC constexpr const auto RFLAGS_MBS { 1ull << 1 };
    PUBLIC constexpr const auto RFLAGS_IF_1 { 1ull << 9 };
    PUBLIC constexpr const auto RFLAGS_IF_0 { 0ull << 9 };
    PUBLIC constexpr const auto RFLAGS_IOPL_0 { 0ull << 12 };
    PUBLIC constexpr const auto RFLAGS_IOPL_3 { 3ull << 12 };

    /* ------------------------------------------------------------------------------------
     * IDT内部：
     * 0 ~ 20   异常入口
     * 21 ~ 26  保留
     * 27 ~ 29  异常入口
     * 30 ~ 256 中断入口
        - 30 ~ 31 无用处
        - 32 时间中断
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
        - 51 APIC ERROR
        - 63 APIC SPURIOUS
        - 128 系统调用
     * ------------------------------------------------------------------------------------ */

    // -
    PUBLIC constexpr const auto IDT_DESC_P { 1 };
    PUBLIC constexpr const auto IDT_DESC_DPL0 { 0 };
    PUBLIC constexpr const auto IDT_DESC_DPL3 { 3 };
    PUBLIC constexpr const auto IDT_DESC_64_TYPE { 0xF };     // 64 位的门
    PUBLIC constexpr const auto IDT_DESC_32_TYPE { 0xE };     // 32 位的门
    PUBLIC constexpr const auto IDT_DESC_16_TYPE { 0x6 };     // 16 位的门

    PUBLIC constexpr const auto INTERRUPT_DESCRIPTOR_TABLE_COUNT { 1 };
    PUBLIC constexpr const auto INTERRUPT_DESCRIPTOR_COUNT { 256 };
    PUBLIC constexpr const auto GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT { 32 };
    PUBLIC constexpr const auto SEGMENT_DESCRIPTOR_COUNT { 8192 };
    PUBLIC constexpr const auto TASK_STATE_SEGMENT_DESCRIPTOR_COUNT { GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT };
    PUBLIC constexpr const auto LOCAL_DESCRIPTOR_COUNT { GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT * SEGMENT_DESCRIPTOR_COUNT - TASK_STATE_SEGMENT_DESCRIPTOR_COUNT };

    /** @brief -------------最大CORE数量 **/
    PUBLIC constexpr const auto CORE_COUNT { GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT };

    PUBLIC constexpr const auto IDT_ENTRY_IRQ_0 { 0x20 };
    PUBLIC constexpr const auto IRQ_CLOCK { IDT_ENTRY_IRQ_0 };
    PUBLIC constexpr const auto IRQ_KEYBOARD { IDT_ENTRY_IRQ_0 + 0x01 };
    PUBLIC constexpr const auto IRQ_CASCADE { IDT_ENTRY_IRQ_0 + 0x02 };
    PUBLIC constexpr const auto IRQ_CMOS_RTC { IDT_ENTRY_IRQ_0 + 0x08 };
    PUBLIC constexpr const auto IRQ_LOCAL_APIC_SPURIOUS { IDT_ENTRY_IRQ_0 + 0x1F };
    PUBLIC constexpr const auto IRQ_LOCAL_APIC_ERROR { IDT_ENTRY_IRQ_0 + 0x13 };
    PUBLIC constexpr const auto IRQ_SYSTEM_CALL { IDT_ENTRY_IRQ_0 + 0x60 };

    // APIC
    //
    PUBLIC constexpr CONST auto LOCAL_APIC_ID { 0x0020 / 4 };          // 编号
    PUBLIC constexpr CONST auto LOCAL_APIC_VER { 0x0030 / 4 };         // 版本
    PUBLIC constexpr CONST auto LOCAL_APIC_TPR { 0x0080 / 4 };         // 任务优先级
    PUBLIC constexpr CONST auto LOCAL_APIC_EOI { 0x00B0 / 4 };         // 意向书
    PUBLIC constexpr CONST auto LOCAL_APIC_SVR { 0x00F0 / 4 };         // 杂散中断向量
    PUBLIC constexpr CONST auto LOCAL_APIC_ENABLE { 0x00000100 };      // 单位使能
    PUBLIC constexpr CONST auto LOCAL_APIC_ESR { 0x0280 / 4 };         // 错误状态
    PUBLIC constexpr CONST auto LOCAL_APIC_ICRLO { 0x0300 / 4 };       // 中断命令
    PUBLIC constexpr CONST auto LOCAL_APIC_INIT { 0x00000500 };        // 初始化/重置
    PUBLIC constexpr CONST auto LOCAL_APIC_STARTUP { 0x00000600 };     // 启动 IPI
    PUBLIC constexpr CONST auto LOCAL_APIC_DELIVS { 0x00001000 };      // 交货状态
    PUBLIC constexpr CONST auto LOCAL_APIC_ASSERT { 0x00004000 };      // 断言中断 （vs deassert）
    PUBLIC constexpr CONST auto LOCAL_APIC_DEASSERT { 0x00000000 };
    PUBLIC constexpr CONST auto LOCAL_APIC_LEVEL { 0x00008000 };     // 级别触发
    PUBLIC constexpr CONST auto LOCAL_APIC_BCAST { 0x00080000 };     // 发送给所有 APIC，包括自己。
    PUBLIC constexpr CONST auto LOCAL_APIC_BUSY { 0x00001000 };
    PUBLIC constexpr CONST auto LOCAL_APIC_FIXED { 0x00000000 };
    PUBLIC constexpr CONST auto LOCAL_APIC_ICRHI { 0x0310 / 4 };        // 中断命令 [63：32]
    PUBLIC constexpr CONST auto LOCAL_APIC_TIMER { 0x0320 / 4 };        // 局部向量表 0 （TIMER）
    PUBLIC constexpr CONST auto LOCAL_APIC_X1 { 0x0000000B };           // 将计数除以 1
    PUBLIC constexpr CONST auto LOCAL_APIC_PERIODIC { 0x00020000 };     // 周期的
    PUBLIC constexpr CONST auto LOCAL_APIC_PCINT { 0x0340 / 4 };        // 性能计数器 LVT
    PUBLIC constexpr CONST auto LOCAL_APIC_LINT0 { 0x0350 / 4 };        // 本地向量表 1 （LINT0）
    PUBLIC constexpr CONST auto LOCAL_APIC_LINT1 { 0x0360 / 4 };        // 本地向量表 2 （LINT1）
    PUBLIC constexpr CONST auto LOCAL_APIC_ERROR { 0x0370 / 4 };        // 本地向量表 3 （ERROR）
    PUBLIC constexpr CONST auto LOCAL_APIC_MASKED { 0x00010000 };       // 中断屏蔽
    PUBLIC constexpr CONST auto LOCAL_APIC_TICR { 0x0380 / 4 };         // 计时器初始计数
    PUBLIC constexpr CONST auto LOCAL_APIC_TCCR { 0x0390 / 4 };         // 定时器电流计数
    PUBLIC constexpr CONST auto LOCAL_APIC_TDCR { 0x03E0 / 4 };         // 定时器分频配置
    PUBLIC constexpr CONST auto IOAPIC_REG_ID { 0x00 };
    PUBLIC constexpr CONST auto IOAPIC_REG_VER { 0x01 };
    PUBLIC constexpr CONST auto IOAPIC_REG_TABLE { 0x10 };
    /* 重定向表从 REG_TABLE 开始，使用两个寄存器来配置每个中断。一对中的第一个（低电平）寄存器包含配置位。 第二个（高）寄存器包含一个位掩码，告诉哪个CPU 可以提供该中断。 */

    PUBLIC constexpr CONST auto INT_DISABLED { 0x00010000 };      // 中断已禁用
    PUBLIC constexpr CONST auto INT_LEVEL { 0x00008000 };         // 电平触发（与边沿-）
    PUBLIC constexpr CONST auto INT_ACTIVELOW { 0x00002000 };     // 低电平有效（与高电平相比）
    PUBLIC constexpr CONST auto INT_LOGICAL { 0x00000800 };       // 目标为 CPU ID（与 APIC ID 相对）
    // 8259A

    PUBLIC constexpr CONST auto PIC_M_CTRL { 0x20 }; /* 主 PIC 的 IO 基址 */
    PUBLIC constexpr CONST auto PIC_S_CTRL { 0xA0 }; /* 从 PIC 的 IO 基址 */
    PUBLIC constexpr CONST auto PIC_M_COMMAND { PIC_M_CTRL };
    PUBLIC constexpr CONST auto PIC_M_DATA { ( PIC_M_CTRL + 1 ) }; /* 主数据 */
    PUBLIC constexpr CONST auto PIC_S_COMMAND { PIC_S_CTRL };
    PUBLIC constexpr CONST auto PIC_S_DATA { ( PIC_S_CTRL + 1 ) }; /* 从数据 */
    PUBLIC constexpr CONST auto PIC_EOI { 0x20 };                  /* 中断结束 */
    PUBLIC constexpr CONST auto ICW1 { 0x11 };                     /* 中断控制命令字PIC用于初始化 */
    PUBLIC constexpr CONST auto ICW1_ICW4 { 0x01 };                /* 表示ICW4将出现 */
    PUBLIC constexpr CONST auto ICW1_SINGLE { 0x02 };              /* 单（级联）模式 */
    PUBLIC constexpr CONST auto ICW1_INTERVAL4 { 0x04 };           /* 呼叫地址间隔4（8） */
    PUBLIC constexpr CONST auto ICW1_LEVEL { 0x08 };               /* 电平触发（边缘）模式 */
    PUBLIC constexpr CONST auto ICW1_INIT { 0x10 };                /* 初始化-必需！*/
    PUBLIC constexpr CONST auto ICW4_8086 { 0x01 };                /* 8086/88（MCS-80/85）模式 */
    PUBLIC constexpr CONST auto ICW4_AUTO { 0x02 };                /* 自动（正常）EOI */
    PUBLIC constexpr CONST auto ICW4_BUF_SLAVE { 0x08 };           /* 缓冲模式/从片 */
    PUBLIC constexpr CONST auto ICW4_BUF_MASTER { 0x0C };          /* 缓冲模式/主片 */
    PUBLIC constexpr CONST auto ICW4_SFNM { 0x10 };                /* 特殊完全嵌套（非） */
    PUBLIC constexpr CONST auto PIC_READ_IRR { 0x0a };             /* OCW3 irq准备好下一次CMD读取 */
    PUBLIC constexpr CONST auto PIC_READ_ISR { 0x0b };             /* OCW3 irq服务下一次CMD读取 */

    // 8254 PIT

    PUBLIC constexpr CONST auto IRQ0_FREQUENCY { 100 };
    PUBLIC constexpr CONST auto INPUT_FREQUENCY { 1193180 };
    PUBLIC constexpr CONST auto COUNTER0_VALUE { INPUT_FREQUENCY / IRQ0_FREQUENCY };
    PUBLIC constexpr CONST auto CONTRER0_PORT { 0x40 };
    PUBLIC constexpr CONST auto COUNTER0_NO { 0 };
    PUBLIC constexpr CONST auto COUNTER_MODE { 2 };
    PUBLIC constexpr CONST auto READ_WRITE_LATCH { 3 };
    PUBLIC constexpr CONST auto PIT_CONTROL_PORT { 0x43 };
}     // namespace QuantumNEC::Architecture::Platform