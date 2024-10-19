#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/sound/arch/x86_64/pcspeaker.hpp>
#elif defined( __aarch64__ )
#endif
PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class SoundArch :
#if defined( __x86_64__ )
        public PCSpeaker
#elif defined( __aarch64__ )
#endif
    {
    public:
        explicit SoundArch( VOID ) noexcept = default;
    };
}