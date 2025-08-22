#pragma once
#include <concepts>
#include <kernel/driver/acpi/table.hpp>
#include <kernel/memory/paging/hhdm.hpp>
namespace Driver {
struct [[gnu::packed]] XSDT : Table::ACPISDTHeader {
    explicit XSDT( void ) noexcept = default;

    auto operator[]( std::size_t index ) const -> const ACPISDTHeader & {
        auto entries { reinterpret_cast< const uint64_t * >( this + 1 ) };
        return *reinterpret_cast< const ACPISDTHeader * >( entries + index );
    }
    auto size( void ) const {
        return ( this->length - sizeof( XSDT ) ) / sizeof( uint64_t );
    }

    template < typename TableType >
        requires std::invocable< decltype( TableType::get_signature ) >
    auto find_table( ) -> TableType * {
        auto address = reinterpret_cast< uint64_t * >( this + 1 );
        for ( std::size_t i { }; i < this->size( ); i++ ) {
            auto *header = (ACPISDTHeader *)( Memory::physical_to_virtual( address[ i ] ) );
            if ( header->signature == TableType::get_signature( ) ) {
                if ( Table::check_sum( header, header->length ) )
                    return reinterpret_cast< TableType::table_type * >( header );
            }
        }
        // 没找到这个表
        return nullptr;
    }
    consteval static auto get_signature( ) {
        return Table::SIGN_32( 'X', 'S', 'D', 'T' );
    }
};

}     // namespace Driver