#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PIC::PIC( VOID ) noexcept :
#ifdef APIC
        Apic {
    }
#else
        PIC8259A { }
#endif
    {
    }
}