#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class SlabCreater
    {
    public:
        explicit SlabCreater( VOID ) noexcept = default;
        virtual ~SlabCreater( VOID ) noexcept = default;

    public:
        auto create( IN uint64_t size, IN FuncPtr< VOID *, VOID *, uint64_t > constructor, IN FuncPtr< VOID *, VOID *, uint64_t > destructor ) -> SlabCache *;
    };
}