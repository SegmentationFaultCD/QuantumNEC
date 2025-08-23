#pragma once
#include <concepts>
#include <cstdint>
namespace Driver {
template < typename Bit >
    requires std::integral< Bit >
class MMIO {
public:
    explicit MMIO( auto address ) :
        mmio_address { (std::uint64_t)address } {
    }
    ~MMIO( void ) = default;

public:
    auto operator[]( std::uint64_t offset ) const -> const Bit & {
        return *( (const Bit *)( this->mmio_address + offset ) );
    }
    auto operator[]( std::uint64_t offset ) -> Bit & {
        return *( (Bit *)( this->mmio_address + offset ) );
    }

private:
    std::uint64_t mmio_address;
};
}     // namespace Driver
