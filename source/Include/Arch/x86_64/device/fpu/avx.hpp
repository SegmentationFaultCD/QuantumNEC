#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    class Avx
    {
    public:
        explicit Avx( VOID ) noexcept;
        virtual ~Avx( VOID ) noexcept = default;
    };
}
