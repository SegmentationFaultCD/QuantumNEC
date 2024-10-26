#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class SlabCollector
    {
    public:
        explicit SlabCollector( VOID ) noexcept;
        virtual ~SlabCollector( VOID ) noexcept = default;

    public:
        auto free( IN uint64_t size ) -> VOID *;
    };
}