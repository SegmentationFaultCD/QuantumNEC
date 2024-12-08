#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class SlabDestroyer {
public:
    explicit SlabDestroyer( void ) noexcept = default;
    virtual ~SlabDestroyer( void ) noexcept = default;

public:
    auto destory( IN SlabCache *slab_cache ) -> void;
};
}     // namespace QuantumNEC::Kernel