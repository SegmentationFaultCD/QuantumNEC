#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class SlabCollector
    {
    public:
        explicit SlabCollector( VOID ) noexcept = default;
        virtual ~SlabCollector( VOID ) noexcept = default;

    public:
        auto free( IN SlabCache *slab_cache, IN VOID *address, IN uint64_t arg ) -> VOID;
    };
}