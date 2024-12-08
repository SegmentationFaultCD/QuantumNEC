#include <kernel/interrupt/x86_64/pit/pit.hpp>

namespace QuantumNEC::Kernel::x86_64 {
PIT::PIT( void ) noexcept :
#ifndef APIC
    P8254 {
}
#else
    Hpet { }
#endif
{
}
PIT::~PIT( void ) noexcept {
}
}     // namespace QuantumNEC::Kernel::x86_64