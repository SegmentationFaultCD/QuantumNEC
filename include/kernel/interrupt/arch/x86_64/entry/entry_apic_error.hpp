#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class ApicErrorEntry
    {
    public:
        explicit ApicErrorEntry( VOID ) noexcept;
        virtual ~ApicErrorEntry( VOID ) noexcept = default;

    private:
    };
}
