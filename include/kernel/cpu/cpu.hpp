#pragma once
#include <lib/Uefi.hpp>
#include <kernel/cpu/arch/cpu_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class CPU :
        public CpuArch
    {
    public:
        explicit CPU( VOID ) noexcept = default;
        ~CPU( VOID ) noexcept = default;
    };
}