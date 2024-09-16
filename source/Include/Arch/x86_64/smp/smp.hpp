#pragma once
#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class SymmetricMultiprocessing
    {
    public:
        explicit SymmetricMultiprocessing( VOID ) noexcept;
        virtual ~SymmetricMultiprocessing( VOID ) noexcept = default;
    };
}
