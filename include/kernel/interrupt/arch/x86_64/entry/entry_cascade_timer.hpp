#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Cascade_TimerEntry
    {
    public:
        explicit Cascade_TimerEntry( VOID ) noexcept;
        virtual ~Cascade_TimerEntry( VOID ) noexcept = default;

        inline STATIC uint64_t global_jiffies { };
    };
}
