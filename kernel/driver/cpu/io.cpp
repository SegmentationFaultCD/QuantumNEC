#include <kernel/driver/cpu/io.hpp>
#include <utility>
using namespace Driver;

auto IO::cpuid( CpuidStatus &status ) -> CpuidStatus & {
    uint64_t max_basic_operation_code { };
    __asm__ __volatile__( "cpuid" : "=a"( max_basic_operation_code ) : "a"( status.mop & 0x80000000 ) : "rbx", "rcx", "rdx" );
    if ( status.mop <= max_basic_operation_code ) {
        __asm__ __volatile__( "cpuid\n\t" : "=a"( status.rax ), "=b"( status.rbx ), "=c"( status.rcx ), "=d"( status.rdx ) : "0"( status.mop ), "2"( status.sop ) );
    }
    return status;
}

auto IO::cli( void ) -> void {
    __asm__ __volatile__( "cli\n\t" ::: "memory" );
}

auto IO::sti( void ) -> void {
    __asm__ __volatile__( "sti\n\t" ::: "memory" );
}

auto IO::hlt( void ) -> void {
    __asm__ __volatile__( "hlt\n\t" ::: "memory" );
}

auto IO::nop( void ) -> void {
    __asm__ __volatile__( "nop\n\t" ::: "memory" );
}

auto IO::rdmsr( uint64_t address ) -> uint64_t {
    uint32_t tmp0 { };
    uint32_t tmp1 { };
    __asm__ __volatile__( "rdmsr	\n\t" : "=d"( tmp0 ), "=a"( tmp1 ) : "c"( address ) : "memory" );
    return static_cast< uint64_t >( tmp0 ) << 32 | tmp1;
}

auto IO::wrmsr( uint64_t address,
                uint64_t value )
    -> void {
    __asm__ __volatile__( "wrmsr	\n\t" ::"d"( value >> 32 ), "a"( value & 0xffffffff ), "c"( address ) : "memory" );
}

auto IO::get_rsp( void ) -> uint64_t {
    uint64_t rsp { };
    __asm__ __volatile__( "movq	%%rsp, %0	\n\t" : "=r"( rsp )::"memory" );
    return rsp;
}

auto IO::get_rflags( void ) -> uint64_t {
    uint64_t rsp_flags { };
    __asm__ __volatile__( "pushfq	\n\t"
                          "movq	(%%rsp), %0	\n\t"
                          "popfq	\n\t" : "=r"( rsp_flags ) : : "memory" );
    return rsp_flags;
}
auto IO::in8( uint16_t port ) -> uint8_t {
    uint8_t ret { };
    __asm__ __volatile__( "inb	%%dx,	%0	\n\t"
                          "mfence			\n\t" : "=a"( ret ) : "d"( port ) : "memory" );
    return ret;
}
auto IO::in16( uint16_t port ) -> uint16_t {
    uint16_t ret { };
    __asm__ __volatile__( "inw	%%dx,	%0	\n\t"
                          "mfence			\n\t" : "=a"( ret ) : "d"( port ) : "memory" );
    return ret;
}

auto IO::in32( uint16_t port ) -> uint32_t {
    uint32_t ret { };
    __asm__ __volatile__( "inl	%%dx,	%0	\n\t"
                          "mfence			\n\t" : "=a"( ret ) : "d"( port ) : "memory" );
    return ret;
}

auto IO::out8( uint16_t port,
               uint8_t  value )
    -> void {
    __asm__ __volatile__( "outb %b[value],%w[port];" : : [value] "a"( value ), [port] "d"( port ) : "memory" );
}

auto IO::out16( uint16_t port,
                uint16_t value )
    -> void {
    __asm__ __volatile__( "outw %w[value],%w[port];" : : [value] "a"( value ), [port] "d"( port ) : "memory" );
}

auto IO::out32( uint16_t port,
                uint32_t value )
    -> void {
    __asm__ __volatile__( "outl %[value],%w[port];" : : [value] "a"( value ), [port] "d"( port ) : "memory" );
}
auto IO::read_cr8( void ) -> ControlRegisters::CR8 {
    ControlRegisters::CR8 cr8 { };
    __asm__ __volatile__( "movq %%cr8, %0" : "=r"( cr8 )::"memory" );
    return cr8;
}

auto IO::write_cr8( ControlRegisters::CR8 cr8 ) -> void {
    __asm__ __volatile__( "movq %0, %%cr8" ::"r"( cr8 ) : "memory" );
}
auto IO::read_cr4( void ) -> ControlRegisters::CR4 {
    ControlRegisters::CR4 cr4 { };
    __asm__ __volatile__( "movq %%cr4, %0" : "=r"( cr4 )::"memory" );
    return cr4;
}

auto IO::write_cr4( ControlRegisters::CR4 cr4 ) -> void {
    __asm__ __volatile__( "movq %0, %%cr4" ::"r"( cr4 ) : "memory" );
}
auto IO::read_cr3( void ) -> ControlRegisters::CR3 {
    ControlRegisters::CR3 cr3 { };
    __asm__ __volatile__( "movq %%cr3, %0" : "=r"( cr3 )::"memory" );
    return cr3;
}

auto IO::write_cr3( ControlRegisters::CR3 cr3 ) -> void {
    __asm__ __volatile__( "movq %0, %%cr3" ::"r"( cr3 ) : "memory" );
}

auto IO::read_cr2( void ) -> ControlRegisters::CR2 {
    ControlRegisters::CR2 cr2 { };
    __asm__ __volatile__( "movq %%cr2, %0" : "=r"( cr2 )::"memory" );
    return cr2;
}

auto IO::write_cr2( ControlRegisters::CR2 cr2 ) -> void {
    __asm__ __volatile__( "movq %0, %%cr2" ::"r"( cr2 ) : "memory" );
}

auto IO::read_cr0( void ) -> ControlRegisters::CR0 {
    ControlRegisters::CR0 cr0 { };
    __asm__ __volatile__( "movq %%cr0, %0" : "=r"( cr0 )::"memory" );
    return cr0;
}

auto IO::write_cr0( ControlRegisters::CR0 cr0 ) -> void {
    __asm__ __volatile__( "movq %0, %%cr0" ::"r"( cr0 ) : "memory" );
}

auto IO::invlpg( void *address ) -> void {
    __asm__ __volatile__( "invlpg (%0)" : : "r"( address ) : "memory" );
}
auto IO::pause( void ) -> void {
    __asm__ __volatile__( "pause" );
}
auto IO::mfence( void ) -> void {
    __asm__ __volatile__( "mfence" ::: "memory" );
}
auto IO::lfence( void ) -> void {
    __asm__ __volatile__( "lfence" ::: "memory" );
}
auto IO::sfence( void ) -> void {
    __asm__ __volatile__( "sfence" ::: "memory" );
}
auto IO::set_page_table( uint64_t *mmap ) -> void {
    ControlRegisters::CR3 cr3 { };
    cr3.page_directory_base = (uint64_t)mmap >> 12;
    write_cr3( cr3 );
}
auto IO::get_page_table( void ) -> uint64_t * {
    return reinterpret_cast< uint64_t * >( read_cr3( ).page_directory_base << 12 );
}
auto IO::cpuid( CpuidStatus &&status ) -> CpuidStatus {
    CpuidStatus   new_status { std::move( status ) };
    std::uint64_t max_basic_operation_code { };
    __asm__ __volatile__( "cpuid" : "=a"( max_basic_operation_code ) : "a"( status.mop & 0x80000000 ) : "rbx", "rcx", "rdx" );
    if ( status.mop <= max_basic_operation_code ) {
        __asm__ __volatile__( "cpuid\n\t" : "=a"( new_status.rax ), "=b"( new_status.rbx ), "=c"( new_status.rcx ), "=d"( new_status.rdx ) : "0"( new_status.mop ), "2"( new_status.sop ) );
    }
    return status;
}