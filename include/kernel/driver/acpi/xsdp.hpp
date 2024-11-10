#pragma once
#include <lib/Uefi.hpp>
#include <libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    STATIC auto check_sum( IN CONST VOID * buffer, IN size_t size ) {
        uint8_t sum { };
        for ( size_t i = 0; i < size; i++ ) {
            sum += ( reinterpret_cast< char_t * >( const_cast< VOID * >( buffer ) ) )[ i ];
        }
        return !sum;
    };
    STATIC consteval auto SIGN_16( auto A, auto B ) -> uint16_t {
        return ( A ) | ( B << 8u );
    }
    STATIC consteval auto SIGN_32( auto A, auto B, auto C, auto D ) -> uint32_t {
        return SIGN_16( A, B ) | ( SIGN_16( C, D ) << 16u );
    }
    STATIC consteval auto SIGN_64( auto A, auto B, auto C, auto D, auto E, auto F, auto G, auto H ) -> uint64_t {
        return (uint64_t)SIGN_32( A, B, C, D ) | ( (uint64_t)SIGN_32( E, F, G, H ) << 32u );
    }
    PUBLIC class Xsdp {
    public:
        struct _packed XSDP {
            char_t   signature[ 8 ];
            uint8_t  check_sum;
            char_t   OEMID[ 6 ];
            uint8_t  revision;
            uint32_t rsdt_address;
            uint32_t length;
            uint64_t xsdt_address;
            uint8_t  extended_check_sum;
            char_t   reserved[ 3 ];
            explicit XSDP( VOID ) noexcept = default;
        };

    public:
        auto bytes_sum_total( IN CONST uint8_t *data, IN size_t bytes ) {
            uint8_t sum { };
            for ( size_t i { }; i < bytes; ++i ) {
                sum += data[ i ];
            }
            return sum;
        }
        explicit Xsdp( IN uint64_t xsdp_address ) noexcept;

        auto get_rsdp( VOID ) -> XSDP * {
            if ( this->xsdp ) {
                return this->xsdp;
            }

            return NULL;
        }

    public:
        constexpr static auto signature { SIGN_64( 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' ) };

    private:
        inline STATIC XSDP *xsdp;
    };
}