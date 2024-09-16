#pragma once
#include <Boot/base.hpp>

namespace QuantumNEC::Boot {
class MemoryService
{
private:
    EFI_MEMORY_DESCRIPTOR *Buffer;
    uint64_t MemorySize;
    uint64_t MemoryCount;
    uint64_t MemoryKey;
    uint64_t DescriptorSize;
    uint32_t DescriptorVersion;

public:
    explicit MemoryService( VOID ) noexcept;
    ~MemoryService( VOID ) noexcept {
    }

public:
    constexpr auto get_memory_key( VOID ) {
        return MemoryKey;
    }

private:
};
}     // namespace QuantumNEC::Boot
