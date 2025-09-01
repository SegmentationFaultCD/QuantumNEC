#pragma once
#include <cstdint>
#include <limine.h>
namespace Driver {
class Table {
public:
    consteval static auto SIGN_16( auto A, auto B ) -> uint16_t {
        return ( A ) | ( B << 8u );
    }
    consteval static auto SIGN_32( auto A, auto B, auto C, auto D ) -> uint32_t {
        return SIGN_16( A, B ) | ( SIGN_16( C, D ) << 16u );
    }
    consteval static auto SIGN_64( auto A, auto B, auto C, auto D, auto E, auto F, auto G, auto H ) -> uint64_t {
        return (uint64_t)SIGN_32( A, B, C, D ) | ( (uint64_t)SIGN_32( E, F, G, H ) << 32u );
    }
    static auto check_sum( const void *buffer, std::size_t size ) {
        uint8_t sum { };
        for ( std::size_t i = 0; i < size; i++ ) {
            sum += ( reinterpret_cast< char * >( const_cast< void * >( buffer ) ) )[ i ];
        }
        return !sum;
    };
    static auto bytes_sum_total( const std::uint8_t *data, std::size_t bytes ) {
        std::uint8_t sum { };
        for ( std::size_t i { }; i < bytes; ++i ) {
            sum += data[ i ];
        }
        return sum;
    }
    struct [[gnu::packed]] ACPISDTHeader {
        std::uint32_t signature;
        std::uint32_t length;
        std::uint8_t revision;
        std::uint8_t check_sum;
        std::uint8_t OEMID[ 6 ];
        std::uint8_t OEMTableID[ 8 ];
        std::uint32_t OEM_revision;
        std::uint32_t creator_ID;
        std::uint32_t creator_revision;
    };
    struct [[gnu::packed]] GenericAddressStructure {
        std::uint8_t address_space_id;     // 0 - system memory, 1 - system I/O
        std::uint8_t register_bit_width;
        std::uint8_t register_bit_offset;
        std::uint8_t access_width;
        std::uint64_t address;
    };
};
auto initialize_acpi( limine_rsdp_response *rsdp ) -> void;
}     // namespace Driver