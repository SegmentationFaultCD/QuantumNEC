#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Arch/x86_64/device/fpu.hpp>
#include <Arch/x86_64/device/graphics.hpp>
#include <Arch/x86_64/device/acpi.hpp>
#include <Arch/x86_64/device/audio.hpp>
#include <Arch/x86_64/device/beep.hpp>
#include <Arch/x86_64/device/cmos.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Device :
        public Graphics,
        public Acpi,
        public Beep,
        public CMOS,
        public FPU
    {
    public:
        explicit Device( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;
        virtual ~Device( VOID ) noexcept;

    private:
    };
}