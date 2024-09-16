#pragma once
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class ClockEntry
    {
    public:
        explicit ClockEntry( VOID ) noexcept;
        virtual ~ClockEntry( VOID ) noexcept = default;

    private:
    };
}
