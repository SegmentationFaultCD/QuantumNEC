#pragma once
#include <kernel/interrupt/x86_64/entry/entry.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class InterruptEntryRegister {
public:
    explicit InterruptEntryRegister( void ) noexcept;

public:
    inline static uint64_t global_jiffies { };

public:
    inline static InterruptEntry *entry[ INTERRUPT_ENTRY_COUNT ];
};
}     // namespace QuantumNEC::Kernel::x86_64