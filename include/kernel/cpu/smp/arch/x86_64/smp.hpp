#pragma once
#include <kernel/interrupt/arch/x86_64/pic/apic.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class SymmetricMultiprocessing {
    public:
        explicit SymmetricMultiprocessing( VOID ) noexcept;
        virtual ~SymmetricMultiprocessing( VOID ) noexcept = default;

    public:
        STATIC auto send_IPI( IN Apic::InterruptCommandRegister icr ) -> VOID;

    public:
        STATIC auto switch_cpu( VOID ) -> VOID;
    };
    PUBLIC [[noreturn]] auto apu_entry( IN limine_smp_info * cpu )->VOID;
}
