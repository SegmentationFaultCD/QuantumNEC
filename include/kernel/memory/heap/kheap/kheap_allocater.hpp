#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class KHeapAllocater {
    public:
        explicit KHeapAllocater( VOID ) noexcept = default;
        virtual ~KHeapAllocater( VOID ) noexcept = default;

    public:
        auto allocate( IN uint64_t size ) -> VOID *;
    };
}