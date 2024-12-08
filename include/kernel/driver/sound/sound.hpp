#pragma once

#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/sound/x86_64/pcspeaker.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class Sound :
#if defined( __x86_64__ )
    public x86_64::PCSpeaker
#elif defined( __aarch64__ )
#endif
{
public:
    explicit Sound( void ) noexcept = default;
    virtual ~Sound( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel