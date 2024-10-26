#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class SlabDestroyer
    {
    public:
        explicit SlabDestroyer( VOID ) noexcept;
        virtual ~SlabDestroyer( VOID ) noexcept = default;

    public:
        auto destory( IN SlabCache *slab_cache ) -> VOID *;
    };
}