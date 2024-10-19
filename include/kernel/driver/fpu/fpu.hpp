#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/fpu/arch/fpu_arch.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class FPU : public FpuArch
    {
    public:
        explicit FPU( VOID ) noexcept;
        virtual ~FPU( VOID ) noexcept = default;

    public:
    };
}
