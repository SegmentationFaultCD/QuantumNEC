#include <kernel/cpu/cpu.hpp>
#include <kernel/cpu/fpu/x86_64/sse.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
using namespace QuantumNEC::Kernel::x86_64;

auto Sse::SSE::check_fpu( void ) -> bool {
    CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
    CPU::cpuid( status );
    if ( !( status.rdx & CPU::CpuidStatus::CPUID_FEAT_RDX_FPU ) ) {
        return false;
    }
    return true;
}
auto Sse::SSE::save( void ) -> void {
    ASM( "FXSAVE %0" ::"m"( *this ) );
}

auto Sse::SSE::load( void ) -> void {
    ASM( "FXRSTOR %0" ::"m"( *this ) );
}
Sse::Sse( void ) noexcept {
    this->activate( );
    println< print_level::OK >( "Initialize the Streaming SIMD Extensions(SSE)" );
}

auto Sse::activate( void ) noexcept -> void {
    auto cr0 = CPU::read_cr0( );
    auto cr4 = CPU::read_cr4( );
    cr0.EM   = 0;
    cr0.MP   = 1;
    CPU::write_cr0( cr0 );
    cr4.OSFXSR     = 1;
    cr4.OSXMMEXCPT = 1;
    CPU::write_cr4( cr4 );
}