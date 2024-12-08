#pragma once
#include <kernel/interrupt/x86_64/pic/apic.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class SymmetricMultiprocessing {
public:
    explicit SymmetricMultiprocessing( void ) noexcept;
    virtual ~SymmetricMultiprocessing( void ) noexcept = default;

public:
    static auto send_IPI( IN Apic::InterruptCommandRegister icr ) -> void;

public:
    static auto switch_cpu( void ) -> void;
};
[[noreturn]] auto apu_entry( IN limine_smp_info *cpu ) -> void;
}     // namespace QuantumNEC::Kernel::x86_64
