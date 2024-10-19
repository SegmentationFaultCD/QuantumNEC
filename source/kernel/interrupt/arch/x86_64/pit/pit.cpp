#include <kernel/interrupt/arch/x86_64/pit/pit.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PIT::PIT( VOID ) noexcept :
#ifndef APIC
        P8254 {
    }
#else
        Hpet { }
#endif
    {
    }
    PIT::~PIT( VOID ) noexcept {
    }
}