#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class SlabCollector {
public:
    explicit SlabCollector( void ) noexcept = default;
    virtual ~SlabCollector( void ) noexcept = default;

public:
    auto free( IN SlabCache *slab_cache, IN void *address, IN uint64_t arg ) -> void;
};
}     // namespace QuantumNEC::Kernel