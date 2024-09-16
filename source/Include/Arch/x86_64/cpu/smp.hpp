#pragma once
#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/Base/deflib.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class SymmetricMultiprocessing
    {
    public:
        explicit( TRUE ) SymmetricMultiprocessing( VOID ) noexcept;
        virtual ~SymmetricMultiprocessing( VOID ) noexcept = default;
    };
}
