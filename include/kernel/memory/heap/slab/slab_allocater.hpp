#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class SlabAllocater {
public:
    explicit SlabAllocater( void ) noexcept = default;
    virtual ~SlabAllocater( void ) noexcept = default;

public:
    auto allocate( IN SlabCache *slab_cache, IN uint64_t arg ) -> void *;
};
}     // namespace QuantumNEC::Kernel