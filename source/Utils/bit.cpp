#include <Utils/bit.hpp>
PUBLIC namespace QuantumNEC::Utils {
    PUBLIC auto bit_set( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t {
        return *addr | ( 1UL << nr );
    }

    PUBLIC auto bit_get( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t {
        return *addr & ( 1UL << nr );
    }
    PUBLIC auto bit_clean( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t {
        return *addr & ( ~( 1UL << nr ) );
    }
}