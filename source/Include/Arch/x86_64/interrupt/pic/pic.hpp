#pragma once
#include <Arch/x86_64/interrupt/pic/apic.hpp>
#include <Arch/x86_64/interrupt/pic/8259a.hpp>

PUBLIC namespace QuantumNEC::Architecture {
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