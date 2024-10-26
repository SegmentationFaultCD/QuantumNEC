#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class SlabAllocater
    {
    public:
        explicit SlabAllocater( VOID ) noexcept;
        virtual ~SlabAllocater( VOID ) noexcept = default;

    public:
        auto allocate( IN SlabCache *slab_cache, IN uint64_t arg ) -> VOID *;
    };
}