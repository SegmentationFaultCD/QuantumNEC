#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/cpu/fpu/arch/x86_64/avx.hpp>
#include <kernel/cpu/fpu/arch/x86_64/sse.hpp>
#elif defined( __aarch64__ )
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class FloatPointUnit :
#if defined( __x86_64__ )
        public Sse,
        public Avx
#elif defined( __aarch64__ )

#endif
    {
    public:
        explicit FloatPointUnit( VOID ) noexcept = default;

    public:
        using FpuFrame = SSE;
    };
}