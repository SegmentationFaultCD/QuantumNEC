#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/xsdp.hpp>
#include <concepts>
#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    class Xsdt
    {
    public:
        struct ACPISDTHeader
        {
            uint32_t signature;
            uint32_t length;
            uint8_t reserved;
            uint8_t check_sum;
            uint8_t OEMID[ 6 ];
            uint8_t OEMTableID[ 8 ];
            uint32_t OEM_reserved;
            uint32_t creator_ID;
            uint32_t creator_reserved;
            explicit ACPISDTHeader( VOID ) noexcept = default;
        } _packed;
        struct GenericAddressStructure
        {
            uint8_t address_space_id;     // 0 - system memory, 1 - system I/O
            uint8_t register_bit_width;
            uint8_t register_bit_offset;
            uint8_t access_width;
            uint64_t address;
            explicit GenericAddressStructure( VOID ) noexcept = default;
        } _packed;

        struct XSDT : ACPISDTHeader
        {
            explicit XSDT( VOID ) noexcept = default;
            auto operator[]( size_t index ) CONST->CONST ACPISDTHeader & {
                auto entries { reinterpret_cast< CONST uint64_t * >( this + 1 ) };
                return *reinterpret_cast< CONST ACPISDTHeader * >( entries + index );
            }
            auto size( VOID ) CONST {
                return ( this->length - sizeof( XSDT ) ) / sizeof( uint64_t );
            }
        } _packed;

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
                    if ( check_sum( header, header->length ) )
                        return reinterpret_cast< TableType::table_type * >( header );
                }
            }
            // 没找到这个表
            return NULL;
        }

    public:
        inline STATIC constexpr auto signature { SIGN_32( 'X', 'S', 'D', 'T' ) };

    private:
        inline STATIC XSDT *xsdt;
    };
}