#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/device/fpu/sse.hpp>
#include <Arch/x86_64/device/fpu/avx.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class FPU :
        public Sse,
        public Avx
    {
    public:
        explicit FPU( VOID ) noexcept :
            Sse { }, Avx { } {
        }
        virtual ~FPU( VOID ) noexcept = default;

    public:
    };
}
