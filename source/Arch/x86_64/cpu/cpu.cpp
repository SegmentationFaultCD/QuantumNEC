#include <Arch/x86_64/platform/platform.hpp>
#include <utility>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace std;
    CPUs::CPUs( VOID ) noexcept {
        // 初始化GDT， IDT
        STATIC byte_t buffer[ sizeof( InterruptDescriptorTable ) + sizeof( GlobalSegmentDescriptorTable ) ] { };
        auto p { buffer };
        this->idt = new ( p ) InterruptDescriptorTable { };
        p += sizeof( InterruptDescriptorTable );
        this->gdt = new ( p ) GlobalSegmentDescriptorTable { };

        // CPU Management初始化
        println< ostream::HeadLevel::OK >( "Initialize the CPU management." );

        using enum DisplayColor;
        CpuidStatus status { };
        char_t factoryName[ 17 ] { };

        status.mop = 0, status.sop = 0;

        this->cpuid( status );
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] ) = status.rbx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] ) = status.rdx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] ) = status.rcx;
        factoryName[ 12 ] = '\0';
        println< ostream::HeadLevel::INFO >( "CPU device information is as follows:" );
        println< ostream::HeadLevel::INFO >( "{} {:x} {:x} {:x}         ", factoryName, status.rbx, status.rdx, status.rcx );

        // 获取处理器商标信息

        char_t factory_name_str[ 256 ] { };
        for ( uint32_t i { 0x80000002 }; i <= 0x80000004; i++ ) {
            status.mop = i, status.sop = 0;
            this->cpuid( status );
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] ) = status.rax;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] ) = status.rbx;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] ) = status.rcx;
            *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 12 ] ) = status.rdx;
            factoryName[ 16 ] = '\0';
            std::strcat( factory_name_str, factoryName );
        }
        println< ostream::HeadLevel::INFO >( "{}", factory_name_str );
        // 获取处理器的版本信息
        status.mop = 1, status.sop = 0;
        this->cpuid( status );
        println< ostream::HeadLevel::INFO >(
            "Family Code -> {:x}, Extended Family -> {:x}, Model Number:{:x}, Extended Model -> {:x}, Processor Type -> {:x}, Stepping ID -> {:x}",
            ( status.rax >> 8 & 0xf ),       // 处理器商标信息引索值(IA-32处理器特有)
            ( status.rax >> 20 & 0xff ),     // CLFLUSH指令刷新的缓存行容量(单位8B)
            ( status.rax >> 4 & 0xf ),       // 处理器包内最大可寻址逻辑处理器ID
            ( status.rax >> 16 & 0xf ),      // 初始 APIC ID
            ( status.rax >> 12 & 0x3 ),
            ( status.rax & 0xf ) );     // 上面两个都是处理器支持的位功能信息
        // 获取线性/物理地址位宽
        status.mop = 0x80000008, status.sop = 0;
        this->cpuid( status );
        println< ostream::HeadLevel::INFO >( "Physical Address size -> {} | Linear Address size -> {}", ( status.rax & 0xff ), ( status.rax >> 8 & 0xff ) );
        // 获得处理器支持的最大基础功能号
        status.mop = 0, status.sop = 0;
        this->cpuid( status );
        println< ostream::HeadLevel::INFO >( "Max Basic Operation Code -> {:x}", status.rax );
        // 获得处理器支持的最大扩展功能号
        status.mop = 0x80000000, status.sop = 0;
        this->cpuid( status );
        println< ostream::HeadLevel::INFO >( "Max Extended Operation Code -> {:x}", status.rax );
    }
    auto CPUs::cpuid( IN CpuidStatus & status ) -> CpuidStatus & {
        uint64_t max_basic_operation_code { };
        ASM( "cpuid"
             : "=a"( max_basic_operation_code )
             : "a"( status.mop & 0x80000000 )
             : "rbx", "rcx", "rdx" );
        if ( status.mop <= max_basic_operation_code ) {
            ASM( "cpuid\n\t"
                 : "=a"( status.rax ), "=b"( status.rbx ), "=c"( status.rcx ), "=d"( status.rdx )
                 : "0"( status.mop ), "2"( status.sop ) );
        }
        return status;
    }

    auto CPUs::port_insw( IN uint64_t port, IN VOID * buffer, IN uint64_t nr ) -> VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "insw\n\t"
             "mfence\n\r" ::"d"( port ),
             "D"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUs::port_outsw( IN uint64_t port, IN VOID * buffer, IN uint64_t nr ) -> VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "outsw\n\t"
             "mfence\n\r" ::"d"( port ),
             "S"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUs::cli( VOID ) -> VOID {
        ASM( "cli\n\t" ::
                 : "memory" );
    }

    auto CPUs::sti( VOID ) -> VOID {
        ASM( "sti\n\t" ::
                 : "memory" );
    }

    auto CPUs::hlt( VOID ) -> VOID {
        ASM( "hlt\n\t" ::
                 : "memory" );
    }

    auto CPUs::nop( VOID ) -> VOID {
        ASM( "nop\n\t" ::
                 : "memory" );
    }

    auto CPUs::rdmsr( IN uint64_t address ) -> uint64_t {
        uint32_t tmp0 { };
        uint32_t tmp1 { };
        ASM( "rdmsr	\n\t"
             : "=d"( tmp0 ), "=a"( tmp1 )
             : "c"( address )
             : "memory" );
        return static_cast< uint64_t >( tmp0 ) << 32 | tmp1;
    }

    auto CPUs::wrmsr( IN uint64_t address,
                      IN uint64_t value )
        -> VOID {
        ASM( "wrmsr	\n\t" ::"d"( value >> 32 ), "a"( value & 0xffffffff ), "c"( address )
             : "memory" );
    }

    auto CPUs::get_rsp( VOID ) -> uint64_t {
        uint64_t rsp { };
        ASM( "movq	%%rsp, %0	\n\t"
             : "=r"( rsp )::"memory" );
        return rsp;
    }

    auto CPUs::get_rflags( VOID ) -> uint64_t {
        uint64_t rsp_flags { };
        ASM( "pushfq	\n\t"
             "movq	(%%rsp), %0	\n\t"
             "popfq	\n\t"
             : "=r"( rsp_flags )
             :
             : "memory" );
        return rsp_flags;
    }
    auto CPUs::io_in8( IN uint16_t port ) -> uint8_t {
        uint8_t ret { };
        ASM( "inb	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }
    auto CPUs::io_in16( IN uint16_t port ) -> uint16_t {
        uint16_t ret { };
        ASM( "inw	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUs::io_in32( IN uint16_t port ) -> uint32_t {
        uint32_t ret { };
        ASM( "inl	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUs::io_out8( IN uint16_t port,
                        IN uint8_t value )
        -> VOID {
        ASM( "outb %b[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUs::io_out16( IN uint16_t port,
                         IN uint16_t value )
        -> VOID {
        ASM( "outw %w[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUs::io_out32( IN uint16_t port,
                         IN uint32_t value )
        -> VOID {
        ASM( "outl %[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }
    auto CPUs::read_cr8( VOID ) -> ControlRegisters::CR8 {
        ControlRegisters::CR8 cr8 { };
        ASM( "movq %%cr8, %0"
             : "=r"( cr8 )::"memory" );
        return cr8;
    }

    auto CPUs::write_cr8( IN ControlRegisters::CR8 cr8 ) -> VOID {
        ASM( "movq %0, %%cr8" ::"r"( cr8 )
             : "memory" );
    }
    auto CPUs::read_cr4( VOID ) -> ControlRegisters::CR4 {
        ControlRegisters::CR4 cr4 { };
        ASM( "movq %%cr4, %0"
             : "=r"( cr4 )::"memory" );
        return cr4;
    }

    auto CPUs::write_cr4( IN ControlRegisters::CR4 cr4 ) -> VOID {
        ASM( "movq %0, %%cr4" ::"r"( cr4 )
             : "memory" );
    }
    auto CPUs::read_cr3( VOID ) -> ControlRegisters::CR3 {
        ControlRegisters::CR3 cr3 { };
        ASM( "movq %%cr3, %0"
             : "=r"( cr3 )::"memory" );
        return cr3;
    }

    auto CPUs::write_cr3( IN ControlRegisters::CR3 cr3 ) -> VOID {
        ASM( "movq %0, %%cr3" ::"r"( cr3 )
             : "memory" );
    }

    auto CPUs::read_cr2( VOID ) -> ControlRegisters::CR2 {
        ControlRegisters::CR2 cr2 { };
        ASM( "movq %%cr2, %0"
             : "=r"( cr2 )::"memory" );
        return cr2;
    }

    auto CPUs::write_cr2( IN ControlRegisters::CR2 cr2 ) -> VOID {
        ASM( "movq %0, %%cr2" ::"r"( cr2 )
             : "memory" );
    }

    auto CPUs::read_cr0( VOID ) -> ControlRegisters::CR0 {
        ControlRegisters::CR0 cr0 { };
        ASM( "movq %%cr0, %0"
             : "=r"( cr0 )::"memory" );
        return cr0;
    }

    auto CPUs::write_cr0( IN ControlRegisters::CR0 cr0 ) -> VOID {
        ASM( "movq %0, %%cr0" ::"r"( cr0 )
             : "memory" );
    }

    auto CPUs::invlpg( IN VOID * address ) -> VOID {
        ASM( "invlpg (%0)"
             :
             : "r"( address )
             : "memory" );
    }
    auto CPUs::pause( VOID ) -> VOID {
        ASM( "pause" );
    }
    auto CPUs::mfence( VOID ) -> VOID {
        ASM( "mfence" ::
                 : "memory" );
    }
    auto CPUs::lfence( VOID ) -> VOID {
        ASM( "lfence" ::
                 : "memory" );
    }
    auto CPUs::sfence( VOID ) -> VOID {
        ASM( "sfence" ::
                 : "memory" );
    }
    auto CPUs::set_page_table( IN uint64_t * mmap ) -> VOID {
        ControlRegisters::CR3 cr3 { };
        cr3.page_directory_base = (uint64_t)mmap >> 12;
        write_cr3( cr3 );
    }
    auto CPUs::get_page_table( VOID ) -> uint64_t * {
        return reinterpret_cast< uint64_t * >( Architecture::ArchitectureManager< TARGET_ARCH >::read_cr3( ).page_directory_base << 12 );
    }
    auto CPUs::cpuid( IN CpuidStatus && status ) -> CpuidStatus {
        CpuidStatus new_status { std::move( status ) };
        uint64_t max_basic_operation_code { };
        ASM( "cpuid"
             : "=a"( max_basic_operation_code )
             : "a"( status.mop & 0x80000000 )
             : "rbx", "rcx", "rdx" );
        if ( status.mop <= max_basic_operation_code ) {
            ASM( "cpuid\n\t"
                 : "=a"( new_status.rax ), "=b"( new_status.rbx ), "=c"( new_status.rcx ), "=d"( new_status.rdx )
                 : "0"( new_status.mop ), "2"( new_status.sop ) );
        }
        return status;
    }
}