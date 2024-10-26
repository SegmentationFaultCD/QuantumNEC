#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
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