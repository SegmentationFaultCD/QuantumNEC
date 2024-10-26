#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/cpu/arch/x86_64/cpu_utils.hpp>
#elif defined( __aarch64__ )
#include <kernel/cpu/arch/aarch64/cpu_utils.hpp>
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class CpuArch :
#if defined( __x86_64__ )
        public CPUUtils
#elif defined( __aarch64__ )
        using namespace aarch64;
#endif

    {
    public:
        explicit CpuArch( VOID ) noexcept;
        ~CpuArch( VOID ) noexcept = default;
    };
}