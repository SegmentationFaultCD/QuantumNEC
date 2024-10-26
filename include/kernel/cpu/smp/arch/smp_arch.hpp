#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/cpu/smp/arch/x86_64/smp.hpp>
#elif defined( __aarch64__ )
#include <kernel/cpu/smp/arch/aarch64/smp.hpp>
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class SymmetricMultiprocessingArch :
#if defined( __x86_64__ )
        public SymmetricMultiprocessing
#elif defined( __aarch64__ )

#endif
    {
    public:
        explicit SymmetricMultiprocessingArch( VOID ) noexcept = default;
    };
}