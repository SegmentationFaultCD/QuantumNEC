#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/Base/deflib.hpp>
#include <Lib/IO/Stream/iostream>
#include <Kernel/task.hpp>
#include <Kernel/memory.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    CPUs::CPUs( VOID ) noexcept {
        using namespace QuantumNEC::Lib::IO;
        using namespace QuantumNEC::Lib::Types;
        InterruptDescriptorTable idt { };
        GlobalSegmentDescriptorTable gdt { };
        // CPU Management初始化
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the CPU management." << Lib::IO::endl;
        using enum DisplayColor;
        CpuidStatus status { };
        char_t factoryName[ 17 ] { };
        status.mop = 0, status.sop = 0;
        this->cpuid( status );
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] ) = status.rbx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] ) = status.rdx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] ) = status.rcx;
        factoryName[ 12 ] = '\0';
        sout[ ostream::HeadLevel::INFO ] << "CPU device information is as follows:" << endl;
        sout[ ostream::HeadLevel::SYSTEM ].printk( WHITE, BLACK, "%s\t%#010x\t%#010x\t%#010x\n", factoryName, status.rbx, status.rdx, status.rcx );
        // 获取处理器商标信息
        sout[ ostream::HeadLevel::SYSTEM ];
        for ( uint32_t i { 0x80000002 }; i <= 0x80000004; i++ ) {
            status.mop = i, status.sop = 0;
            this->cpuid( status );
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] ) = status.rax;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] ) = status.rbx;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] ) = status.rcx;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 12 ] ) = status.rdx;
            factoryName[ 16 ] = '\0';
            sout.printk( WHITE, BLACK, "%s", factoryName );
        }
        endl( sout );
        // 获取处理器的版本信息
        status.mop = 1, status.sop = 0;
        this->cpuid( status );
        sout[ ostream::HeadLevel::SYSTEM ].printk( WHITE, BLACK,
                                                   "Family Code -> %#010x, Extended Family -> %#010x | Model Number:%#010x | Extended Model -> %#010x | Processor Type -> %#010x | Stepping ID -> %#010x\n",
                                                   ( status.rax >> 8 & 0xf ),       // 处理器商标信息引索值(IA-32处理器特有)
                                                   ( status.rax >> 20 & 0xff ),     // CLFLUSH指令刷新的缓存行容量(单位8B)
                                                   ( status.rax >> 4 & 0xf ),       // 处理器包内最大可寻址逻辑处理器ID
                                                   ( status.rax >> 16 & 0xf ),      // 初始 APIC ID
                                                   ( status.rax >> 12 & 0x3 ),
                                                   ( status.rax & 0xf ) );     // 上面两个都是处理器支持的位功能信息
        // 获取线性/物理地址位宽

        status.mop = 0x80000008, status.sop = 0;
        this->cpuid( status );
        sout[ ostream::HeadLevel::SYSTEM ].printk( WHITE, BLACK, "Physical Address size -> %08d | Linear Address size -> %08d\n", ( status.rax & 0xff ), ( status.rax >> 8 & 0xff ) );
        // 获得处理器支持的最大基础功能号
        status.mop = 0, status.sop = 0;
        this->cpuid( status );
        sout[ ostream::HeadLevel::SYSTEM ].printk( WHITE, BLACK, "Max Basic Operation Code -> %#010x  | ", status.rax );
        // 获得处理器支持的最大扩展功能号
        status.mop = 0x80000000, status.sop = 0;
        this->cpuid( status );
        sout.printk( WHITE, BLACK, "Max Extended Operation Code -> %#010x\n", status.rax );
    }
    auto CPUs::cpuid( IN CpuidStatus & status )->CpuidStatus & {
        Lib::Types::uint64_t max_basic_operation_code { };
        ASM( "cpuid" : "=a"( max_basic_operation_code ) : "a"( status.mop & 0x80000000 ) : "rbx", "rcx", "rdx" );
        if ( status.mop <= max_basic_operation_code ) {
            ASM( "cpuid\n\t"
                 : "=a"( status.rax ), "=b"( status.rbx ), "=c"( status.rcx ), "=d"( status.rdx )
                 : "0"( status.mop ), "2"( status.sop ) );
        }
        return status;
    }
    auto CPUs::switch_to( IN OUT Lib::Types::Ptr< VOID > current, IN Lib::Types::Ptr< VOID > next )->VOID {
        using Kernel::Task;
        auto current_pcb { reinterpret_cast< Lib::Types::Ptr< Kernel::Thread > >( current ) }, next_pcb { reinterpret_cast< Lib::Types::Ptr< Kernel::Thread > >( next ) };
        ASM( "PUSHQ %1\n\t"
             "PUSHQ %0\n\t"
             "LEAQ _asm_thread_switch_to(%%RIP), %%RAX\n\t"
             "CALLQ *%%RAX\n\t" : : "g"( &current_pcb->cpu_frame ), "g"( &next_pcb->cpu_frame ) : );
    }
    auto CPUs::port_insw( IN Lib::Types::uint64_t port, IN Lib::Types::Ptr< VOID > buffer, IN Lib::Types::uint64_t nr )->VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "insw\n\t"
             "mfence\n\r" ::"d"( port ),
             "D"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUs::port_outsw( IN Lib::Types::uint64_t port, IN Lib::Types::Ptr< VOID > buffer, IN Lib::Types::uint64_t nr )->VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "outsw\n\t"
             "mfence\n\r" ::"d"( port ),
             "S"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUs::cli( VOID )->VOID {
        ASM( "cli\n\t" ::: "memory" );
    }

    auto CPUs::sti( VOID )->VOID {
        ASM( "sti\n\t" ::: "memory" );
    }

    auto CPUs::hlt( VOID )->VOID {
        ASM( "hlt\n\t" ::: "memory" );
    }

    auto CPUs::nop( VOID )->VOID {
        ASM( "nop\n\t" ::: "memory" );
    }

    auto CPUs::rdmsr( IN Lib::Types::uint64_t address )->Lib::Types::uint64_t {
        Lib::Types::uint32_t tmp0 { };
        Lib::Types::uint32_t tmp1 { };
        ASM( "rdmsr	\n\t"
             : "=d"( tmp0 ), "=a"( tmp1 )
             : "c"( address )
             : "memory" );
        return static_cast< Lib::Types::uint64_t >( tmp0 ) << 32 | tmp1;
    }

    auto CPUs::wrmsr( IN Lib::Types::uint64_t address,
                      IN Lib::Types::uint64_t value )
        ->VOID {
        ASM( "wrmsr	\n\t" ::"d"( value >> 32 ), "a"( value & 0xffffffff ), "c"( address )
             : "memory" );
    }

    auto CPUs::get_rsp( VOID )->Lib::Types::uint64_t {
        Lib::Types::uint64_t rsp { };
        ASM( "movq	%%rsp, %0	\n\t"
             : "=r"( rsp )::"memory" );
        return rsp;
    }

    auto CPUs::get_rflags( VOID )->Lib::Types::uint64_t {
        Lib::Types::uint64_t rsp_flags { };
        ASM( "pushfq	\n\t"
             "movq	(%%rsp), %0	\n\t"
             "popfq	\n\t"
             : "=r"( rsp_flags )
             :
             : "memory" );
        return rsp_flags;
    }
    auto CPUs::io_in8( IN Lib::Types::uint16_t port )->Lib::Types::uint8_t {
        Lib::Types::uint8_t ret { };
        ASM( "inb	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }
    auto CPUs::io_in16( IN Lib::Types::uint16_t port )->Lib::Types::uint16_t {
        Lib::Types::uint16_t ret { };
        ASM( "inw	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUs::io_in32( IN Lib::Types::uint16_t port )->Lib::Types::uint32_t {
        Lib::Types::uint32_t ret { };
        ASM( "inl	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUs::io_out8( IN Lib::Types::uint16_t port,
                        IN Lib::Types::uint8_t value )
        ->VOID {
        ASM( "outb %b[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUs::io_out16( IN Lib::Types::uint16_t port,
                         IN Lib::Types::uint16_t value )
        ->VOID {
        ASM( "outw %w[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUs::io_out32( IN Lib::Types::uint16_t port,
                         IN Lib::Types::uint32_t value )
        ->VOID {
        ASM( "outl %[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUs::read_cr4( VOID )->ControlRegisters::CR4 {
        ControlRegisters::CR4 cr4 { };
        ASM( "movq %%cr4, %0" : "=r"( cr4 )::"memory" );
        return cr4;
    }

    auto CPUs::write_cr4( IN ControlRegisters::CR4 cr4 )->VOID {
        ASM( "movq %0, %%cr4" ::"r"( cr4 ) : "memory" );
    }
    auto CPUs::read_cr3( VOID )->ControlRegisters::CR3 {
        ControlRegisters::CR3 cr3 { };
        ASM( "movq %%cr3, %0" : "=r"( cr3 )::"memory" );
        return cr3;
    }

    auto CPUs::write_cr3( IN ControlRegisters::CR3 cr3 )->VOID {
        ASM( "movq %0, %%cr3" ::"r"( cr3 ) : "memory" );
    }

    auto CPUs::read_cr2( VOID )->ControlRegisters::CR2 {
        ControlRegisters::CR2 cr2 { };
        ASM( "movq %%cr2, %0" : "=r"( cr2 )::"memory" );
        return cr2;
    }

    auto CPUs::write_cr2( IN ControlRegisters::CR2 cr2 )->VOID {
        ASM( "movq %0, %%cr2" ::"r"( cr2 ) : "memory" );
    }

    auto CPUs::read_cr0( VOID )->ControlRegisters::CR0 {
        ControlRegisters::CR0 cr0 { };
        ASM( "movq %%cr0, %0" : "=r"( cr0 )::"memory" );
        return cr0;
    }

    auto CPUs::write_cr0( IN ControlRegisters::CR0 cr0 )->VOID {
        ASM( "movq %0, %%cr0" ::"r"( cr0 ) : "memory" );
    }

    auto CPUs::invlpg( IN Lib::Types::Ptr< VOID > address )->VOID {
        ASM( "invlpg (%0)" : : "r"( address ) : "memory" );
    }
    auto CPUs::pause( VOID )->VOID {
        ASM( "pause" );
    }
    auto CPUs::mfence( VOID )->VOID {
        ASM( "mfence" ::: "memory" );
    }
    auto CPUs::lfence( VOID )->VOID {
        ASM( "lfence" ::: "memory" );
    }
    auto CPUs::sfence( VOID )->VOID {
        ASM( "sfence" ::: "memory" );
    }
    auto CPUs::set_page_table( IN Lib::Types::Ptr< Lib::Types::uint64_t > mmap )->VOID {
        ASM( "movq %0, %%cr3\n\t" ::"r"( mmap ) );
        // write_cr3(  );
    }
}