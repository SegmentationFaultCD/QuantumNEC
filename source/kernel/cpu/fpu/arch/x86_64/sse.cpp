#include <kernel/cpu/cpu.hpp>
#include <kernel/cpu/fpu/arch/x86_64/sse.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
using namespace QuantumNEC::Kernel;

auto Sse::SSE::check_fpu( VOID ) -> BOOL {
    CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
    CPU::cpuid( status );
    if ( !( status.rdx & CPU::CpuidStatus::CPUID_FEAT_RDX_FPU ) ) {
        return FALSE;
    }
    return TRUE;
}
auto Sse::SSE::save_sse( VOID ) -> VOID {
    ASM( "FXSAVE %0" ::"m"( *this ) );
}

auto Sse::SSE::load_sse( VOID ) -> VOID {
    ASM( "FXRSTOR %0" ::"m"( *this ) );
}
Sse::Sse( VOID ) noexcept {
    this->activate_sse( );
    println< ostream::HeadLevel::OK >( "Initialize the Streaming SIMD Extensions(SSE)" );
}

auto Sse::activate_sse( VOID ) noexcept -> VOID {
    auto cr0 = CPU::read_cr0( );
    auto cr4 = CPU::read_cr4( );
    cr0.EM   = 0;
    cr0.MP   = 1;
    CPU::write_cr0( cr0 );
    cr4.OSFXSR     = 1;
    cr4.OSXMMEXCPT = 1;
    CPU::write_cr4( cr4 );
}