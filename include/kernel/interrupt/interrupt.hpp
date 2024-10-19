#pragma once
#ifndef APIC
#define APIC
#endif
#include <lib/Uefi.hpp>
#include <kernel/interrupt/arch/interrupt_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Interrupt : public InterruptArch
    {
    public:
        explicit Interrupt( VOID ) noexcept = default;
    };
}