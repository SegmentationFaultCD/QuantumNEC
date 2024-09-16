#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC using namespace QuantumNEC::Architecture;
    PUBLIC class LocalApicEntry
    {
    public:
        explicit LocalApicEntry( VOID ) noexcept;
        virtual ~LocalApicEntry( VOID ) noexcept = default;

    private:
    };
}
