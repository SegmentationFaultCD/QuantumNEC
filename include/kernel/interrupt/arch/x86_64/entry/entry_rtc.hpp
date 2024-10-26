#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class RTCEntry
    {
    public:
        explicit RTCEntry( VOID ) noexcept;
        virtual ~RTCEntry( VOID ) noexcept = default;

    private:
    };
}
