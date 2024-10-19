#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/rsdp.hpp>
#include <concepts>
#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    class Rsdt
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
        struct RSDT : ACPISDTHeader
        {
            explicit RSDT( VOID ) noexcept = default;
            auto operator[]( size_t index ) CONST->CONST ACPISDTHeader & {
                auto entries { reinterpret_cast< CONST uint32_t * >( this + 1 ) };
                return *reinterpret_cast< CONST ACPISDTHeader * >( entries + index );
            }
            auto size( VOID ) CONST {
                return ( this->length - sizeof( RSDT ) ) / sizeof( uint32_t );
            }
        } _packed;
        struct GenericAddressStructure
        {
            explicit GenericAddressStructure( VOID ) noexcept = default;
            uint8_t address_space_id;     // 0 - system memory, 1 - system I/O
            uint8_t register_bit_width;
            uint8_t register_bit_offset;
            uint8_t access_width;
            uint64_t address;
        } _packed;

        explicit Rsdt( IN Rsdp &rsdp ) noexcept;

    public:
        template < typename TableType >
            requires requires {
                typename TableType::table_type;
                { TableType::signature } -> std::same_as< const uint32_t >;
            } && std::constructible_from< TableType, Rsdt >
        auto find_table( ) {
            auto address = reinterpret_cast< uint32_t * >( this->rsdt + 1 );
            for ( size_t i { }; i < this->rsdt->size( ); i++ ) {
                auto *header = (ACPISDTHeader *)( uint64_t( address[ i ] ) );
                if ( header->signature == TableType::signature ) {
                    if ( Rsdp::check_sum( header, header->length ) )
                        return reinterpret_cast< TableType::table_type * >( header );
                }
            }
            // 没找到这个表
            return reinterpret_cast< TableType::table_type * >( NULL );
        }

    public:
        constexpr static auto signature { SIGN_32( 'R', 'S', 'D', 'T' ) };

    private:
        inline STATIC RSDT *rsdt;
    };
}