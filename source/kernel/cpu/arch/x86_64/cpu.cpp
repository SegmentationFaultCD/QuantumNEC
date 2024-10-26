#include <kernel/cpu/arch/x86_64/cpu_utils.hpp>
#include <utility>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace std;

    auto CPUUtils::cpuid( IN CpuidStatus & status ) -> CpuidStatus & {
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

    auto CPUUtils::port_insw( IN uint64_t port, IN VOID * buffer, IN uint64_t nr ) -> VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "insw\n\t"
             "mfence\n\r" ::"d"( port ),
             "D"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUUtils::port_outsw( IN uint64_t port, IN VOID * buffer, IN uint64_t nr ) -> VOID {
        ASM( "cld\n\t"
             "rep\n\t"
             "outsw\n\t"
             "mfence\n\r" ::"d"( port ),
             "S"( buffer ), "c"( nr )
             : "memory" );
    }

    auto CPUUtils::cli( VOID ) -> VOID {
        ASM( "cli\n\t" ::
                 : "memory" );
    }

    auto CPUUtils::sti( VOID ) -> VOID {
        ASM( "sti\n\t" ::
                 : "memory" );
    }

    auto CPUUtils::hlt( VOID ) -> VOID {
        ASM( "hlt\n\t" ::
                 : "memory" );
    }

    auto CPUUtils::nop( VOID ) -> VOID {
        ASM( "nop\n\t" ::
                 : "memory" );
    }

    auto CPUUtils::rdmsr( IN uint64_t address ) -> uint64_t {
        uint32_t tmp0 { };
        uint32_t tmp1 { };
        ASM( "rdmsr	\n\t"
             : "=d"( tmp0 ), "=a"( tmp1 )
             : "c"( address )
             : "memory" );
        return static_cast< uint64_t >( tmp0 ) << 32 | tmp1;
    }

    auto CPUUtils::wrmsr( IN uint64_t address,
                          IN uint64_t value )
        -> VOID {
        ASM( "wrmsr	\n\t" ::"d"( value >> 32 ), "a"( value & 0xffffffff ), "c"( address )
             : "memory" );
    }

    auto CPUUtils::get_rsp( VOID ) -> uint64_t {
        uint64_t rsp { };
        ASM( "movq	%%rsp, %0	\n\t"
             : "=r"( rsp )::"memory" );
        return rsp;
    }

    auto CPUUtils::get_rflags( VOID ) -> uint64_t {
        uint64_t rsp_flags { };
        ASM( "pushfq	\n\t"
             "movq	(%%rsp), %0	\n\t"
             "popfq	\n\t"
             : "=r"( rsp_flags )
             :
             : "memory" );
        return rsp_flags;
    }
    auto CPUUtils::io_in8( IN uint16_t port ) -> uint8_t {
        uint8_t ret { };
        ASM( "inb	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }
    auto CPUUtils::io_in16( IN uint16_t port ) -> uint16_t {
        uint16_t ret { };
        ASM( "inw	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUUtils::io_in32( IN uint16_t port ) -> uint32_t {
        uint32_t ret { };
        ASM( "inl	%%dx,	%0	\n\t"
             "mfence			\n\t"
             : "=a"( ret )
             : "d"( port )
             : "memory" );
        return ret;
    }

    auto CPUUtils::io_out8( IN uint16_t port,
                            IN uint8_t value )
        -> VOID {
        ASM( "outb %b[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUUtils::io_out16( IN uint16_t port,
                             IN uint16_t value )
        -> VOID {
        ASM( "outw %w[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }

    auto CPUUtils::io_out32( IN uint16_t port,
                             IN uint32_t value )
        -> VOID {
        ASM( "outl %[value],%w[port];"
             :
             : [value] "a"( value ), [port] "d"( port )
             : "memory" );
    }
    auto CPUUtils::read_cr8( VOID ) -> ControlRegisters::CR8 {
        ControlRegisters::CR8 cr8 { };
        ASM( "movq %%cr8, %0"
             : "=r"( cr8 )::"memory" );
        return cr8;
    }

    auto CPUUtils::write_cr8( IN ControlRegisters::CR8 cr8 ) -> VOID {
        ASM( "movq %0, %%cr8" ::"r"( cr8 )
             : "memory" );
    }
    auto CPUUtils::read_cr4( VOID ) -> ControlRegisters::CR4 {
        ControlRegisters::CR4 cr4 { };
        ASM( "movq %%cr4, %0"
             : "=r"( cr4 )::"memory" );
        return cr4;
    }

    auto CPUUtils::write_cr4( IN ControlRegisters::CR4 cr4 ) -> VOID {
        ASM( "movq %0, %%cr4" ::"r"( cr4 )
             : "memory" );
    }
    auto CPUUtils::read_cr3( VOID ) -> ControlRegisters::CR3 {
        ControlRegisters::CR3 cr3 { };
        ASM( "movq %%cr3, %0"
             : "=r"( cr3 )::"memory" );
        return cr3;
    }

    auto CPUUtils::write_cr3( IN ControlRegisters::CR3 cr3 ) -> VOID {
        ASM( "movq %0, %%cr3" ::"r"( cr3 )
             : "memory" );
    }

    auto CPUUtils::read_cr2( VOID ) -> ControlRegisters::CR2 {
        ControlRegisters::CR2 cr2 { };
        ASM( "movq %%cr2, %0"
             : "=r"( cr2 )::"memory" );
        return cr2;
    }

    auto CPUUtils::write_cr2( IN ControlRegisters::CR2 cr2 ) -> VOID {
        ASM( "movq %0, %%cr2" ::"r"( cr2 )
             : "memory" );
    }

    auto CPUUtils::read_cr0( VOID ) -> ControlRegisters::CR0 {
        ControlRegisters::CR0 cr0 { };
        ASM( "movq %%cr0, %0"
             : "=r"( cr0 )::"memory" );
        return cr0;
    }

    auto CPUUtils::write_cr0( IN ControlRegisters::CR0 cr0 ) -> VOID {
        ASM( "movq %0, %%cr0" ::"r"( cr0 )
             : "memory" );
    }

    auto CPUUtils::invlpg( IN VOID * address ) -> VOID {
        ASM( "invlpg (%0)"
             :
             : "r"( address )
             : "memory" );
    }
    auto CPUUtils::pause( VOID ) -> VOID {
        ASM( "pause" );
    }
    auto CPUUtils::mfence( VOID ) -> VOID {
        ASM( "mfence" ::
                 : "memory" );
    }
    auto CPUUtils::lfence( VOID ) -> VOID {
        ASM( "lfence" ::
                 : "memory" );
    }
    auto CPUUtils::sfence( VOID ) -> VOID {
        ASM( "sfence" ::
                 : "memory" );
    }
    auto CPUUtils::set_page_table( IN uint64_t * mmap ) -> VOID {
        ControlRegisters::CR3 cr3 { };
        cr3.page_directory_base = (uint64_t)mmap >> 12;
        write_cr3( cr3 );
    }
    auto CPUUtils::get_page_table( VOID ) -> uint64_t * {
        return reinterpret_cast< uint64_t * >( read_cr3( ).page_directory_base << 12 );
    }
    auto CPUUtils::cpuid( IN CpuidStatus && status ) -> CpuidStatus {
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