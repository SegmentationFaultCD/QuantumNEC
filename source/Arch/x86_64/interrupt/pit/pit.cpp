#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
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