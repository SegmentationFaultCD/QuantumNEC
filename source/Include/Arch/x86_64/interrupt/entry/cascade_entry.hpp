#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC using namespace QuantumNEC::Architecture;
    PUBLIC class CascadeEntry
    {
    public:
        explicit CascadeEntry( VOID ) noexcept;
        virtual ~CascadeEntry( VOID ) noexcept = default;

    private:
    };
}
