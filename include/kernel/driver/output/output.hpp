#pragma once
#include <kernel/driver/output/arch/output_arch.hpp>
#include <kernel/driver/output/display/graphics.hpp>
#include <lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    class Output : public Graphics, public OutputArch {
    public:
        explicit Output( VOID ) noexcept = default;
        virtual ~Output( VOID ) noexcept = default;
    };
}