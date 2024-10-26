#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/time/arch/x86_64/cmos.hpp>
#elif defined( __aarch64__ )
#include <kernel/driver/time/arch/x86_64/.hpp>
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class TimeArch :
#if defined( __x86_64__ )
        public CMOS
#elif defined( __aarch64__ )

#endif
    {
    public:
        explicit TimeArch( VOID ) noexcept :
            CMOS { } {
        }
    };
}