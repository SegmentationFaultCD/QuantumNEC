#include "Arch/x86_64/cpu/cpu.hpp"
#include "Lib/deflib.hpp"
#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
using namespace QuantumNEC::Kernel;

auto Architecture::Sse::SSE::check_fpu( VOID ) -> BOOL {
    CPUs::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
    CPUs::cpuid( status );
    if ( !( status.rdx & CPUs::CpuidStatus::CPUID_FEAT_RDX_FPU ) ) {
        return FALSE;
    }
    return TRUE;
}
auto Architecture::Sse::SSE::save_sse( VOID ) -> VOID {
    ASM( "FXSAVE %0" ::"m"( *this ) );
}

auto Architecture::Sse::SSE::load_sse( VOID ) -> VOID {
    ASM( "FXRSTOR %0" ::"m"( *this ) );
}
Architecture::Sse::Sse( VOID ) noexcept {
    this->activate_sse( );
    println< ostream::HeadLevel::OK >( "Initialize the Streaming SIMD Extensions(SSE)" );
}

auto Architecture::Sse::activate_sse( VOID ) noexcept -> VOID {
    auto cr0 = CPUs::read_cr0( );
    auto cr4 = CPUs::read_cr4( );
    cr0.EM = 0;
    cr0.MP = 1;
    CPUs::write_cr0( cr0 );
    cr4.OSFXSR = 1;
    cr4.OSXMMEXCPT = 1;
    CPUs::write_cr4( cr4 );
}