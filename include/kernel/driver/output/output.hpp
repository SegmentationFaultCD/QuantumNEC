#pragma once
#include <kernel/driver/output/arch/output_arch.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class Output : public OutputArch {
    public:
        explicit Output( VOID ) noexcept = default;
        virtual ~Output( VOID ) noexcept = default;
    };
}