#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __heap_allocater
    {
    public:
        explicit __heap_allocater( VOID ) noexcept;
        virtual ~__heap_allocater( VOID ) noexcept = default;

    public:
        auto __allocate( IN uint64_t __size ) -> VOID *;
    };
}