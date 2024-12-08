#pragma once
#include <kernel/interrupt/x86_64/pic/8259a.hpp>
#include <kernel/interrupt/x86_64/pic/apic.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class PIC :
#ifdef APIC
    public Apic
#else
    public PIC8259A
#endif
{
public:
    explicit PIC( void ) noexcept;
    virtual ~PIC( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel::x86_64