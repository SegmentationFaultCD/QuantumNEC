#pragma once
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC using namespace QuantumNEC::Architecture;
    PUBLIC class SystemcallEntry
    {
    public:
        explicit SystemcallEntry( VOID ) noexcept;
        virtual ~SystemcallEntry( VOID ) noexcept = default;

    private:
    };
}