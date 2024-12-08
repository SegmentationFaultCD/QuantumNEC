#pragma once
#include <lib/Uefi.hpp>

#include <kernel/memory/x86_64/paging/paging.hpp>
#include <kernel/memory/x86_64/paging/ptv.hpp>
#include <kernel/memory/x86_64/segment/gdt.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class Memory :
    public Paging,
    public GlobalSegmentDescriptorTable {
public:
    explicit Memory( void ) noexcept = default;
    virtual ~Memory( void ) noexcept = default;
};

}     // namespace QuantumNEC::Kernel::x86_64
namespace QuantumNEC::Kernel {
using x86_64::physical_to_virtual;
using x86_64::virtual_to_physical;
}     // namespace QuantumNEC::Kernel