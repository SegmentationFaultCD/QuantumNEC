#pragma once
#include <Arch/x86_64/device/fpu/fpu.hpp>
#include <Arch/x86_64/device/display/graphics.hpp>
#include <Arch/x86_64/device/acpi/acpi.hpp>
#include <Arch/x86_64/device/sound/sound.hpp>
#include <Arch/x86_64/device/time/time.hpp>
#include <Arch/x86_64/device/elf.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Device :
        public Graphics,
        public Acpi,
        public Sound,
        public Time,
        public FPU
    {
    public:
        explicit Device( VOID ) noexcept;
        virtual ~Device( VOID ) noexcept;

    private:
    };
}