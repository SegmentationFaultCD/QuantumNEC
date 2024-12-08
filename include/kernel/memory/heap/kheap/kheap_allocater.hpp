#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class KHeapAllocater {
public:
    explicit KHeapAllocater( void ) noexcept = default;
    virtual ~KHeapAllocater( void ) noexcept = default;

public:
    auto allocate( IN uint64_t size ) -> void *;
};
}     // namespace QuantumNEC::Kernel