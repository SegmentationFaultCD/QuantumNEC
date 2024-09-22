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
    ASM( "MOVQ %%CR0,%%RAX \n\t"
         "ANDW $0xfffb,%%AX \n\t"
         "ORW $0x2,%%AX \n\t"
         "MOVQ %%RAX,%%CR0 \n\t"
         "MOVQ %%CR4,%%RAX \n\t"
         "ORW $(3 << 9),%%AX \n\t"
         "MOVQ %%RAX,%%CR4 \n\t" ::: );
}