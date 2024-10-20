#pragma once
#include <lib/Uefi.hpp>
#include <kernel/interrupt/arch/x86_64/pic/apic.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class SymmetricMultiprocessing
    {
    public:
        explicit SymmetricMultiprocessing( VOID ) noexcept;
        virtual ~SymmetricMultiprocessing( VOID ) noexcept = default;

    public:
        STATIC auto send_IPI( IN Apic::InterruptCommandRegister icr ) -> VOID;
    };
    PUBLIC [[noreturn]] auto apu_entry( IN limine_smp_info * cpu )->VOID;
}
