#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC using namespace QuantumNEC::Architecture;
    PUBLIC class RTCEntry
    {
    public:
        explicit RTCEntry( VOID ) noexcept;
        virtual ~RTCEntry( VOID ) noexcept = default;

    private:
    };
}
