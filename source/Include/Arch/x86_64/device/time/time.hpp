#pragma once
#include <Arch/x86_64/device/time/cmos.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    class Time : public CMOS
    {
    public:
        explicit Time( VOID ) noexcept :
            CMOS { } {
        }
    };
}