#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Audio
    {
    public:
        explicit Audio( VOID ) noexcept;
        virtual ~Audio( VOID ) noexcept;
    };
}