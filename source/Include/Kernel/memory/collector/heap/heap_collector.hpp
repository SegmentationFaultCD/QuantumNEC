#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __heap_collector
    {
    public:
        explicit __heap_collector( VOID ) noexcept;
        virtual ~__heap_collector( VOID ) noexcept = default;

    public:
        auto __free( IN VOID *address ) -> VOID;
    };
}