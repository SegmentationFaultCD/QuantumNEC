#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
constexpr auto SIGN_16( char_t A, char_t B ) -> uint16_t {
    return ( A ) | ( B << 8u );
}
constexpr auto SIGN_32( char_t A, char_t B, char_t C, char_t D ) -> uint32_t {
    return SIGN_16( A, B ) | ( SIGN_16( C, D ) << 16u );
}
constexpr auto SIGN_64( char_t A, char_t B, char_t C, char_t D, char_t E, char_t F, char_t G, char_t H ) -> uint64_t {
    return (uint64_t)SIGN_32( A, B, C, D ) | ( (uint64_t)SIGN_32( E, F, G, H ) << 32u );
}

class Rsdp {
public:
    struct _packed RSDP {
        char_t   signature[ 8 ];
        uint8_t  check_sum;
        char_t   OEMID[ 6 ];
        uint8_t  revision;
        uint32_t rsdt_address;
        explicit RSDP( void ) noexcept = default;
    };

public:
    explicit Rsdp( IN uint64_t rsdp_address ) noexcept {
        this->rsdp = (RSDP *)rsdp_address;
    }
    static auto check_sum( IN const void *buffer, IN size_t size ) {
        uint8_t sum { };
        for ( size_t i = 0; i < size; i++ ) {
            sum += ( reinterpret_cast< char_t * >( const_cast< void * >( buffer ) ) )[ i ];
        }
        return !sum;
    };
    auto bytes_sum_total( IN const uint8_t *data, IN size_t bytes ) {
        uint8_t sum { };
        for ( size_t i { }; i < bytes; ++i ) {
            sum += data[ i ];
        }
        return sum;
    }
    auto get_rsdp( void ) -> RSDP * {
        if ( this->rsdp ) {
            return this->rsdp;
        }
        return NULL;
    }

public:
    constexpr static auto signature { SIGN_64( 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' ) };

private:
    inline static RSDP *rsdp;
};
}     // namespace QuantumNEC::Kernel::x86_64