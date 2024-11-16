#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class SymmetricMultiprocessingSwitchCPUEntry {
    public:
        explicit SymmetricMultiprocessingSwitchCPUEntry( VOID ) noexcept;
        virtual ~SymmetricMultiprocessingSwitchCPUEntry( VOID ) noexcept = default;

    private:
    };
}
