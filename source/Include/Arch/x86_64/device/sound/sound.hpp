#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/device/sound/pcspeaker.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    class Sound : public PCSpeaker
    {
    public:
        explicit Sound( VOID ) noexcept :
            PCSpeaker { } {
        }
    };
}