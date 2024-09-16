#pragma once
#include <Lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class SystemcallEntry
    {
    public:
        explicit SystemcallEntry( VOID ) noexcept;
        virtual ~SystemcallEntry( VOID ) noexcept = default;

    private:
    };
}