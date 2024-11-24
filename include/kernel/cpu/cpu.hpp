#pragma once
#include <kernel/cpu/arch/cpu_arch.hpp>
#include <kernel/cpu/fpu/arch/fpu_arch.hpp>
#include <kernel/cpu/smp/arch/smp_arch.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class CPU :
        public FloatPointUnit,
        public CpuArch,
        public SymmetricMultiprocessingArch {
    public:
        explicit CPU( VOID ) noexcept :
            FloatPointUnit( ), CpuArch( ) {};
        ~CPU( VOID ) noexcept = default;
    };
}