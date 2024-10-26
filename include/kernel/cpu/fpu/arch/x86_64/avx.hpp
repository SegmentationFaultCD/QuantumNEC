#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    class Avx
    {
    public:
        explicit Avx( VOID ) noexcept;
        virtual ~Avx( VOID ) noexcept = default;
    };
}
