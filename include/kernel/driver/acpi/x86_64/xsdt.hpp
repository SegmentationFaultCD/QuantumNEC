#pragma once
#include <concepts>
#include <kernel/driver/acpi/x86_64/xsdp.hpp>
#include <kernel/memory/x86_64/paging/ptv.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Xsdt {
public:
    struct _packed ACPISDTHeader {
        uint32_t signature;
        uint32_t length;
        uint8_t  reserved;
        uint8_t  check_sum;
        uint8_t  OEMID[ 6 ];
        uint8_t  OEMTableID[ 8 ];
        uint32_t OEM_reserved;
        uint32_t creator_ID;
        uint32_t creator_reserved;
        explicit ACPISDTHeader( void ) noexcept = default;
    };
    struct _packed GenericAddressStructure {
        uint8_t  address_space_id;     // 0 - system memory, 1 - system I/O
        uint8_t  register_bit_width;
        uint8_t  register_bit_offset;
        uint8_t  access_width;
        uint64_t address;
        explicit GenericAddressStructure( void ) noexcept = default;
    };

    struct _packed XSDT : ACPISDTHeader {
        explicit XSDT( void ) noexcept = default;
        auto operator[]( size_t index ) const -> const ACPISDTHeader & {
            auto entries { reinterpret_cast< const uint64_t * >( this + 1 ) };
            return *reinterpret_cast< const ACPISDTHeader * >( entries + index );
        }
        auto size( void ) const {
            return ( this->length - sizeof( XSDT ) ) / sizeof( uint64_t );
        }
    };

    explicit Xsdt( IN Xsdp &xsdp ) noexcept;

public:
    template < typename TableType >
        requires requires {
            typename TableType::table_type;
            { TableType::signature };
        } && std::constructible_from< TableType, Xsdt & >
    auto find_table( uint32_t signature = TableType::signature ) -> TableType::table_type * {
        auto address = reinterpret_cast< uint64_t * >( this->xsdt + 1 );
        for ( size_t i { }; i < this->xsdt->size( ); i++ ) {
            auto *header = (ACPISDTHeader *)( physical_to_virtual( address[ i ] ) );
            if ( header->signature == signature ) {
                if ( Xsdp::check_sum( header, header->length ) )
                    return reinterpret_cast< TableType::table_type * >( header );
            }
        }
        // 没找到这个表
        return NULL;
    }

public:
    inline static constexpr auto signature { SIGN_32( 'X', 'S', 'D', 'T' ) };

private:
    inline static XSDT *xsdt;
};
}     // namespace QuantumNEC::Kernel::x86_64