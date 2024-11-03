#pragma once
#include <kernel/cpu/arch/cpu_arch.hpp>
#include <kernel/cpu/fpu/arch/fpu_arch.hpp>
#include <kernel/cpu/smp/arch/smp_arch.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class CPU :
        public CpuArch,
        public FloatPointUnit,
        public SymmetricMultiprocessingArch {
    public:
        explicit CPU( VOID ) noexcept = default;
        ~CPU( VOID ) noexcept         = default;
    };
}