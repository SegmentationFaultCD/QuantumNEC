#pragma once
#include <kernel/interrupt/arch/x86_64/pic/apic.hpp>
#include <kernel/interrupt/arch/x86_64/pic/8259a.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class PIC :
#ifdef APIC
        public Apic
#else
        public PIC8259A
#endif
    {
    public:
        explicit PIC( VOID ) noexcept;
        virtual ~PIC( VOID ) noexcept = default;
    };
}