#pragma once
#include <concepts>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Lib {
template < typename Bit >
class MMIO {
public:
    explicit MMIO( auto address ) :
        mmio_address { (uint64_t)address } {
    }
    ~MMIO( void ) = default;

public:
    auto operator[]( uint64_t offset ) const -> const Bit & {
        return *( (const Bit *)( this->mmio_address + offset ) );
    }
    auto operator[]( uint64_t offset ) -> Bit & {
        return *( (Bit *)( this->mmio_address + offset ) );
    }

private:
    uint64_t mmio_address;
};
}     // namespace QuantumNEC::Lib