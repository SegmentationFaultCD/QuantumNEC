#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Avx {
public:
    explicit Avx( void ) noexcept;
    virtual ~Avx( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel::x86_64
