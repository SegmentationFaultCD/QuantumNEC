#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class HeapAllocater
    {
    public:
        explicit HeapAllocater( VOID ) noexcept = default;
        virtual ~HeapAllocater( VOID ) noexcept = default;

    public:
        auto allocate( IN uint64_t size ) -> VOID *;
        
    };
}