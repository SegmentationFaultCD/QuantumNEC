#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class KHeapCollector {
public:
    explicit KHeapCollector( void ) noexcept = default;
    virtual ~KHeapCollector( void ) noexcept = default;

public:
    auto free( IN void *virtual_address ) -> void;
};
}     // namespace QuantumNEC::Kernel