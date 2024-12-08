#include <kernel/interrupt/x86_64/pic/pic.hpp>
namespace QuantumNEC::Kernel::x86_64 {
PIC::PIC( void ) noexcept :
#ifdef APIC
    Apic {
}
#else
    PIC8259A { }
#endif
{
}
}     // namespace QuantumNEC::Kernel::x86_64