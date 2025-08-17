#pragma once
#include <cstdint>
#include <lib/format.hpp>
namespace Driver {
struct ControlRegisters {
    struct CR0 {
        std::uint64_t PE : 1;     // 如果PE=1，则保护模式启动，如果PE=0，则在实模式下运行
        std::uint64_t MP : 1;     // 监视协处理器
        std::uint64_t EM : 1;     // x87 FPU仿真
        std::uint64_t TS : 1;     // 任务已切换
        std::uint64_t ET : 1;     // 扩展类型
        std::uint64_t NE : 1;     // Numeric error
        std::uint64_t : 10;
        std::uint64_t WP : 1;     // 控制开启内存写保护，WP=0开启写保护，对只读页面尝试写入将触发异常，这一机制常常被用来实现写时复制功能
        std::uint64_t : 1;
        std::uint64_t AM : 1;     // 控制启用内存对齐自动检查
        std::uint64_t : 10;
        std::uint64_t NW : 1;     // 控制使用通写还是回写，用于维护内存数据一致性，NW=1关闭该功能，数据一致性即通过MESI协议
        std::uint64_t CD : 1;     // 控制Cache高速缓存功能，CD=1关闭该功能
        std::uint64_t PG : 1;     // 开启分页模式
        explicit CR0( void ) noexcept = default;
        operator std::uint64_t( ) {
            return *( (uint64_t *)this );
        }
    };

    // CR1 保留
    // CPU在尝试访问它时将抛出#UD异常。

    struct CR2 {
        /*
         *   x86架构中的CR2寄存器是页故障线性地址寄存器，也称为页故障地址寄存器（Page Fault Linear Address Register）。当CPU检测到一个页故障（Page Fault）时，它会将故障发生时的线性地址存储在CR2寄存器中，这个地址指向导致页故障的页面。
         *   内核可以通过读取CR2寄存器来获取引起页故障的地址，然后对该地址所在的页面进行处理，比如进行页面的分配或者进行页面的交换。CR2寄存器的值可以在中断或异常处理中读取。
         *   在保护模式下，访问CR2寄存器需要使用特殊的指令，比如MOV CR2, EAX将EAX寄存器的值写入CR2寄存器，或MOV EAX, CR2将CR2寄存器的值读取到EAX寄存器中。在x86_64架构下，CR2寄存器的大小为64位。
         */

        std::uint64_t PFLA;
        explicit CR2( void ) noexcept = default;
        operator std::uint64_t( ) {
            return *( (uint64_t *)this );
        }
    };
    struct CR3 {
        /*
         *   cr3寄存器和MMU密切相关，保存了当前进程所使用的虚拟地址空间的页目录地址，可以说是整个虚拟地址翻译中的顶级指挥棒，在进程空间切换的时候，CR3也将同步切换。
         *   cr3寄存器的高20位用于保存页目录地址，0-11位记录标记位，所以页目录地址必须是4KB的整数倍。
         */

        std::uint64_t : 3;
        std::uint64_t PWT : 1;
        std::uint64_t PCD : 1;
        std::uint64_t : 7;
        std::uint64_t page_directory_base : 52;
        explicit CR3( void ) noexcept = default;
        operator std::uint64_t( ) {
            return *( (uint64_t *)this );
        }
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

        std::uint64_t VME : 1;
        std::uint64_t PVI : 1;
        std::uint64_t TSD : 1;
        std::uint64_t DE : 1;
        std::uint64_t PSE : 1;
        std::uint64_t PAE : 1;
        std::uint64_t MCE : 1;
        std::uint64_t PGE : 1;
        std::uint64_t PCE : 1;
        std::uint64_t OSFXSR : 1;
        std::uint64_t OSXMMEXCPT : 1;
        std::uint64_t UMIP : 1;
        std::uint64_t : 1;
        std::uint64_t VMXE : 1;
        std::uint64_t SMXE : 1;
        std::uint64_t : 1;
        std::uint64_t FSGSBASE : 1;
        std::uint64_t PCIDE : 1;
        std::uint64_t OSXSAVE : 1;
        std::uint64_t : 1;
        std::uint64_t SMEP : 1;
        std::uint64_t SMAP : 1;
        std::uint64_t PKE : 1;
        std::uint64_t CET : 1;
        std::uint64_t PKS : 1;
        std::uint64_t : 39;
        explicit CR4( void ) noexcept = default;
        operator std::uint64_t( ) {
            return *( (uint64_t *)this );
        }
    };
    // CR5 ~ CR7 保留，如果使用结果和 CR1 一样.
    struct CR8 {
        // 任务优先级寄存器
        uint64_t TPL : 4;
        uint64_t : 60;
        explicit CR8( void ) noexcept = default;
        operator std::uint64_t( ) {
            return *( (uint64_t *)this );
        }
    };
    // CR9 ~ CR15 保留，如果使用结果和 CR1 一样.
};

struct [[gnu::packed]] RFlags {
    std::uint64_t CF : 1;
    std::uint64_t MBS : 1;
    std::uint64_t PF : 1;
    std::uint64_t : 1;
    std::uint64_t AF : 1;
    std::uint64_t : 1;
    std::uint64_t ZF : 1;
    std::uint64_t SF : 1;
    std::uint64_t TF : 1;
    std::uint64_t IF : 1;
    std::uint64_t DF : 1;
    std::uint64_t OF : 1;
    std::uint64_t IOPL : 2;
    std::uint64_t NT : 1;
    std::uint64_t : 1;
    std::uint64_t RF : 1;
    std::uint64_t VM : 1;
    std::uint64_t AC : 1;
    std::uint64_t VIF : 1;
    std::uint64_t VIP : 1;
    std::uint64_t ID : 1;
    std::uint64_t : 42;
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
        __asm__ __volatile__( "movq %%cr0, %0\n\t" : "=r"( this->cr0 ) );
        __asm__ __volatile__( "movq %%cr2, %0\n\t" : "=r"( this->cr2 ) );
        __asm__ __volatile__( "movq %%cr3, %0\n\t" : "=r"( this->cr3 ) );
        __asm__ __volatile__( "movq %%cr4, %0\n\t" : "=r"( this->cr4 ) );
        __asm__ __volatile__( "movq %%cr8, %0\n\t" : "=r"( this->cr8 ) );
    }
    // CR1，CR5~CR7, CR9~CR15为保留，无法访问
};
struct [[gnu::packed]] GeneralPurposeRegistersFrame {
    std::uint64_t rax;
    std::uint64_t rbx;
    std::uint64_t rcx;
    std::uint64_t rdx;
    std::uint64_t rbp;
    std::uint64_t rsi;
    std::uint64_t rdi;
    std::uint64_t r8;
    std::uint64_t r9;
    std::uint64_t r10;
    std::uint64_t r11;
    std::uint64_t r12;
    std::uint64_t r13;
    std::uint64_t r14;
    std::uint64_t r15;
    explicit GeneralPurposeRegistersFrame( void ) noexcept = default;
};
struct [[gnu::packed]] SegmentRegisterFrame {
    std::uint64_t ds;
    std::uint64_t es;
    std::uint64_t fs;
    std::uint64_t gs;
    explicit SegmentRegisterFrame( void ) noexcept = default;
};
struct [[gnu::packed]] RegisterFrame {
    std::uint64_t ds;
    std::uint64_t es;
    std::uint64_t fs;
    std::uint64_t gs;
    std::uint64_t rax;
    std::uint64_t rbx;
    std::uint64_t rcx;
    std::uint64_t rdx;
    std::uint64_t rbp;
    std::uint64_t rsi;
    std::uint64_t rdi;
    std::uint64_t r8;
    std::uint64_t r9;
    std::uint64_t r10;
    std::uint64_t r11;
    std::uint64_t r12;
    std::uint64_t r13;
    std::uint64_t r14;
    std::uint64_t r15;
    explicit RegisterFrame( void ) noexcept = default;
};
}     // namespace Driver
