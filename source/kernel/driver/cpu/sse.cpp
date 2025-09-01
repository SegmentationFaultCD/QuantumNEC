#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/cpu/register.hpp>
#include <kernel/driver/cpu/sse.hpp>
namespace Driver {
auto SSE::check_fpu( ) -> bool {
    auto status = IO::cpuid( { 1, 0, 0, 0, 0, 0 } );
    if ( !( status.rdx & IO::CpuidStatus::CPUID_FEAT_RDX_FPU ) ) {
        return false;
    }
    return true;
}
auto SSE::save( void ) -> void {
    __asm__ __volatile__( "FXSAVE %0" : "=m"( this->fxsave_region )::"memory" );
}

auto SSE::write( void ) -> void {
    __asm__ __volatile__( "FXRSTOR %0" ::"m"( this->fxsave_region ) : "memory" );
}
auto initialize_sse( void ) -> void {
    auto cr0 = IO::read_cr0( );
    auto cr4 = IO::read_cr4( );
    cr0.EM = 0;
    cr0.MP = 1;
    IO::write_cr0( cr0 );
    cr4.OSFXSR = 1;
    cr4.OSXMMEXCPT = 1;
    IO::write_cr4( cr4 );
}
}     // namespace Driver