#pragma once
#include <lib/Uefi.hpp>

#include <kernel/cpu/fpu/fpu.hpp>
#include <kernel/cpu/smp/smp.hpp>

#if defined( __x86_64__ )
#include <kernel/cpu/x86_64/x86_64cpu.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class CPU :
#if defined( __x86_64__ )
    public x86_64::CPUUtils,
#elif defined( __aarch64__ )
#endif
    public FloatPointUnit,
    public SymmetricMultiprocessing {
public:
    explicit CPU( void ) noexcept;
    virtual ~CPU( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel