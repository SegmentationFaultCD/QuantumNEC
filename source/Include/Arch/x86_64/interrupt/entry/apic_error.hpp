#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class ApicErrorEntry
    {
    public:
        explicit ApicErrorEntry( VOID ) noexcept;
        virtual ~ApicErrorEntry( VOID ) noexcept = default;

    private:
    };
}
