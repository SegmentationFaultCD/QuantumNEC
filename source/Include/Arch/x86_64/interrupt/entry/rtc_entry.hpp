#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class RTCEntry
    {
    public:
        explicit RTCEntry( VOID ) noexcept;
        virtual ~RTCEntry( VOID ) noexcept = default;

    private:
    };
}
