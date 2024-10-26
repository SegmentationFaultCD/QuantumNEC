#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/sound/arch/sound_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class Sound : public SoundArch
    {
    public:
        explicit Sound( VOID ) noexcept = default;
    };
}