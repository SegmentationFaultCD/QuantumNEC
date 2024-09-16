#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
using namespace QuantumNEC::Kernel;

Architecture::Avx::Avx( VOID ) noexcept {
    ASM(
        "PUSHQ %%RAX\n\t"
        "PUSHQ %%RCX\n\t"
        "PUSHQ %%RDX\n\t"
        "XORQ %%RCX, %%RCX\n\t"
        "XGETBV\n\t"
        "ORW $7, %%EAX\n\t"
        "XSETBV\n\t"
        "POPQ %%RDX\n\t"
        "POPQ %%RCX\n\t"
        "POPQ %%RAX\n\t" );
    println< ostream::HeadLevel::OK >( "Initialize the Advanced Vector Extensions" );
}