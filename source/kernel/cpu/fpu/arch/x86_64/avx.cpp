#include <kernel/cpu/fpu/arch/x86_64/avx.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
using namespace QuantumNEC::Kernel::x86_64;

Avx::Avx( VOID ) noexcept {
    // ASM(
    //     "PUSHQ %RAX\n\t"
    //     "PUSHQ %RCX\n\t"
    //     "PUSHQ %RDX\n\t"
    //     "XORQ %RCX, %RCX\n\t"
    //     "XGETBV\n\t"
    //     "ORL $7, %EAX\n\t"
    //     "XSETBV\n\t"
    //     "POPQ %RDX\n\t"
    //     "POPQ %RCX\n\t"
    //     "POPQ %RAX\n\t" );

    println< ostream::HeadLevel::OK >( "Initialize the Advanced Vector Extensions" );
}