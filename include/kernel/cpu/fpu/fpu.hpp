#pragma once
#include <lib/Uefi.hpp>

#if defined( __x86_64__ )
#include <kernel/cpu/fpu/x86_64/avx.hpp>
#include <kernel/cpu/fpu/x86_64/sse.hpp>
#elif defined( __aarch64__ )
#endif
namespace QuantumNEC::Kernel {
class FloatPointUnit :
#if defined( __x86_64__ )
    public x86_64::Sse,
    public x86_64::Avx
#elif defined( __aarch64__ )
#endif
{
public:
    explicit FloatPointUnit( void ) noexcept = default;
    virtual ~FloatPointUnit( void ) noexcept = default;
    using FpuFrame                           = SSE;
};
}     // namespace QuantumNEC::Kernel
