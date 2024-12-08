#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class SlabCreater {
public:
    explicit SlabCreater( void ) noexcept = default;
    virtual ~SlabCreater( void ) noexcept = default;

public:
    auto create( IN uint64_t size, IN FuncPtr< void *, void *, uint64_t > constructor, IN FuncPtr< void *, void *, uint64_t > destructor ) -> SlabCache *;
};
}     // namespace QuantumNEC::Kernel