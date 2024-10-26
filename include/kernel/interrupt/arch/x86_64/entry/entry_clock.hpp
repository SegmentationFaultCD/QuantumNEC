#pragma once
#include <lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class ClockEntry
    {
    public:
        explicit ClockEntry( VOID ) noexcept;
        virtual ~ClockEntry( VOID ) noexcept = default;

    private:
    };
}
