#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Memory {

Paging::Paging( void ) noexcept {
}
auto Paging::initialize( limine_paging_mode_response *pg ) -> Paging * {
    static Paging paging { };
    if ( pg->mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
        paging.support_5level_paging = true;
        paging.kernel_page_table = new pml5t { (uint64_t)physical_to_virtual( Driver::IO::get_page_table( ) ) };
    }
    else {
        paging.support_5level_paging = false;
        paging.kernel_page_table = new pml4t { (uint64_t)physical_to_virtual( Driver::IO::get_page_table( ) ) };
    }

    paging.kernel_page_table->page_protect( false );
    return &paging;
}

using namespace std;
auto Paging::pmlxt::map( uint64_t physics_address, uint64_t virtual_address, uint64_t size, uint64_t flags, Page::Type mode ) -> void {
    pml1t pml1t { 0 };
    pml2t pml2t { 0 };
    pml3t pml3t { 0 };
    pml4t pml4t { 0 };
    pml5t pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    while ( size-- ) {
        [ & ]( this auto &&self, std::uint64_t level, pmlxt &table ) {
            auto index = table.get_virtual_index( virtual_address );

            if ( !level ) {
                if ( !table.flags_ps_pat( index ) && ( std::to_underlying( mode ) - 1 ) ) {
                    [ & ]( this auto &&_self, std::uint64_t _level, pmlxt &_table, std::uint64_t _index ) {
                        if ( _level == 0 ) {
                            if ( !_table.empty( ) && _table.flags_p( _index ) && !_table.flags_ps_pat( _index ) ) {
                                Page::allocator< Page::Type::P4Kib > { }.deallocate( (void *)_table.flags_base( _index ), 1 );
                            }
                            return;
                        }
                        else {
                            // 往下查找
                            auto &clean_table = *page_table[ _level ];
                            clean_table = (uint64_t *)physical_to_virtual( _table.flags_base( _index ) );

                            auto &next_clean_table = *page_table[ _level - 1 ];

                            for ( auto i = 0; i < 512; ++i ) {
                                next_clean_table = (uint64_t *)physical_to_virtual( clean_table.flags_base( i ) );
                                _self( _level - 1, next_clean_table, i );
                            }
                            Page::allocator< Page::Type::P4Kib > { }.deallocate( (void *)_table.flags_base( _index ), 1 );
                            return;
                        }
                    }( std::to_underlying( mode ) - 2, table, index );
                }

                table = { index,
                          physics_address,
                          flags | table.is_huge( mode ) };
                Driver::IO::invlpg( reinterpret_cast< void * >( virtual_address ) );
                physics_address += table.check_page_size( mode );
                virtual_address += table.check_page_size( mode );

                return;
            }
            else if ( !table.flags_p( index ) || table.flags_ps_pat( index ) ) {
                auto new_table = Page::allocator< Page::Type::P4Kib > { }.allocate( 1 );
                std::memset( (void *)physical_to_virtual( new_table ), 0, table.PT_SIZE );
                table = { index,
                          (uint64_t)new_table,
                          flags };
            }
            auto &next_table = *page_table[ level + std::to_underlying( mode ) - 2 ];
            next_table = (uint64_t *)physical_to_virtual( table.flags_base( index ) );
            self( level - 1, next_table );
            return;
        }( paging->support_5level_paging ? 5 : 4 - std::to_underlying( mode ), *this );
    }
}
auto Paging::pmlxt::unmap( uint64_t virtual_address, std::size_t size, Page::Type mode ) -> void {
    pml1t pml1t { 0 };
    pml2t pml2t { 0 };
    pml3t pml3t { 0 };
    pml4t pml4t { 0 };
    pml5t pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    while ( size-- ) {
        [ & ]( this auto &&self, std::uint64_t level, pmlxt &table ) {
            auto index = table.get_virtual_index( virtual_address );
            auto &next_table = *page_table[ level + std::to_underlying( mode ) - 2 ];
            if ( !table.flags_p( index ) ) {
                return;
            }
            if ( !level || table.flags_ps_pat( index ) ) {
                table.set_p( index, 0 );
                Driver::IO::invlpg( reinterpret_cast< void * >( virtual_address ) );
                virtual_address += table.check_page_size( mode );
                return;
            }
            next_table = (std::uint64_t *)physical_to_virtual( table.flags_base( index ) );
            self( level - 1, next_table );
        }( paging->support_5level_paging ? 5 : 4 - std::to_underlying( mode ), *this );
    }
}

auto Paging::pmlxt::find_physcial_address( std::uint64_t virtual_address, Page::Type mode ) -> void * {
    pml1t pml1t { 0 };
    pml2t pml2t { 0 };
    pml3t pml3t { 0 };
    pml4t pml4t { 0 };
    pml5t pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };
    return [ & ]( this auto &&self, std::uint64_t level, pmlxt &table ) -> void * {
        auto index = table.get_virtual_index( virtual_address );
        if ( !table.flags_p( index ) ) {
            return nullptr;
        }
        if ( !level ) {
            Display::println( "{:x} end", table.get( )[ index ] );
            return (void *)table.flags_base( index );
        }
        else {
            auto &next_table = *page_table[ level + std::to_underlying( mode ) - 2 ];
            next_table = (uint64_t *)physical_to_virtual( table.flags_base( index ) );
            Display::println( "{:x}", table.get( )[ index ] );
            return self( level - 1, next_table );
        }
    }( paging->support_5level_paging ? 5 : 4 - std::to_underlying( mode ), *this );
}
auto Paging::pmlxt::page_protect( bool flags ) -> void {
    auto cr0 = Driver::IO::read_cr0( );
    cr0.WP = !flags;
    Driver::IO::write_cr0( cr0 );
};

auto Paging::pmlxt::activate( void ) -> void {
    if ( this->get( ) ) {
        Driver::IO::set_page_table( (uint64_t *)virtual_to_physical( this->get( ) ) );
    }
}
auto Paging::pmlxt::copy( pmlxt &from ) -> void {
    std::memset( this->get( ), 0, PT_SIZE / 2 );
    std::memcpy( this->get( ) + 256, from.get( ) + 256, PT_SIZE / 2 );
}

Paging::pmlxt::pmlxt( void ) :
    can_allocate { true },
    pt { (uint64_t *)physical_to_virtual( Page::allocator< Page::Type::P4Kib > { }.allocate( 1 ) ) } {
}
Paging::pmlxt::~pmlxt( void ) {
    Page::allocator< Page::Type::P4Kib > table_allocator;
    if ( this->can_allocate ) {
        table_allocator.deallocate( (void *)virtual_to_physical( this->pt ), 1 );
    }
}

}     // namespace Memory