#pragma once
#include <kernel/driver/time/arch/time_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class Time : public TimeArch
    {
    public:
        explicit Time( VOID ) noexcept = default;
    };
}