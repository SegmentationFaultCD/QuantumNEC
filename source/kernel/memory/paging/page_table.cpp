#include <kernel/driver/cpu/io.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Memory {

Paging::Paging( void ) noexcept {
}
auto Paging::initialize( limine_paging_mode_response *pg ) -> void {
    if ( pg->mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
        support_5level_paging = true;
        kernel_page_table     = new pml5t { (uint64_t)Driver::IO::get_page_table( ) };
    }
    else {
        support_5level_paging = false;
        kernel_page_table     = new pml4t { (uint64_t)Driver::IO::get_page_table( ) };
    }

    kernel_page_table->page_protect( false );
}

using namespace std;
auto Paging::pmlxt::map( uint64_t physics_address, uint64_t virtual_address, uint64_t size, uint64_t flags, Page::Type mode ) -> void {
    pml1t  pml1t { 0 };
    pml2t  pml2t { 0 };
    pml3t  pml3t { 0 };
    pml4t  pml4t { 0 };
    pml5t  pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    while ( size-- ) {
        [ & ]( this auto &&self, std::uint64_t level, pmlxt &table ) {
            auto  index      = table.get_virtual_index( virtual_address );
            auto &next_table = *page_table[ level + std::to_underlying( mode ) - 2 ];
            next_table       = (uint64_t *)physical_to_virtual( table.flags_base( index, Page::Type::P4Kib ) );
            if ( !level ) {
                [ & ]( this auto &&self, std::uint64_t _level, pmlxt &table ) {
                    if ( table.empty( ) || level == 0 ) {
                        return;
                    }
                    else {
                        // 往下查找
                        auto &nclean_table = *page_table[ _level - 1 ];
                        for ( auto i = 0; i < 512; ++i ) {
                            nclean_table = (uint64_t *)physical_to_virtual( table.flags_base( i, Page::Type::P4Kib ) );
                            self( _level - 1, nclean_table );
                        }
                        Page::allocator< Page::Type::P4Kib > { }.deallocate( table.get( ), 1 );
                    }
                }( level + std::to_underlying( mode ) - 1, next_table );

                table = { index,
                          physics_address & ~0x7FFul,
                          flags | table.is_huge( mode ),
                          mode };

                physics_address += table.check_page_size( mode );
                virtual_address += table.check_page_size( mode );

                Driver::IO::invlpg( reinterpret_cast< void * >( virtual_address ) );
                return;
            }
            else if ( !table.flags_p( index ) ) {
                auto new_table = Page::allocator< Page::Type::P4Kib > { }.allocate( 1 );
                std::memset( (void *)physical_to_virtual( new_table ), 0, table.PT_SIZE );
                table = { index,
                          (uint64_t)new_table & ~0x7FFul,
                          flags,
                          Page::Type::P4Kib };
            }

            self( level - 1, next_table );
        }( Paging::support_5level_paging ? 5 : 4 - std::to_underlying( mode ), *this );
    }
}
auto Paging::pmlxt::unmap( uint64_t virtual_address, std::size_t size, Page::Type mode ) -> void {
    pml1t  pml1t { 0 };
    pml2t  pml2t { 0 };
    pml3t  pml3t { 0 };
    pml4t  pml4t { 0 };
    pml5t  pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    while ( size-- ) {
        [ & ]( this auto &&self, std::uint64_t level, pmlxt &table ) {
            auto  index      = table.get_virtual_index( virtual_address );
            auto &next_table = *page_table[ level + std::to_underlying( mode ) - 2 ];
            if ( !table.flags_p( index ) ) {
                return;
            }
            if ( !level ) {
                table.set_p( index, 0 );
                Driver::IO::invlpg( reinterpret_cast< void * >( virtual_address ) );
                virtual_address += table.check_page_size( mode );
                return;
            }
            next_table = (std::uint64_t *)physical_to_virtual( table.flags_base( index, Page::Type::P4Kib ) );
            self( level - 1, next_table );
        }( Paging::support_5level_paging ? 5 : 4 - std::to_underlying( mode ), *this );
    }
}

auto Paging::pmlxt::find_physcial_address( void *virtual_address, Page::Type mode ) -> void * {
    pml1t  pml1t { 0 };
    pml2t  pml2t { 0 };
    pml3t  pml3t { 0 };
    pml4t  pml4t { 0 };
    pml5t  pml5t { 0 };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    // auto get_table = [ & ]( uint64_t level ) -> pmlxt & {
    //     return *page_table[ level - 1 ];
    // };

    // auto offset { std::to_underlying( mode ) - 1 };
    // auto level = Paging::support_5level_paging ? 5 : 4;

    // return [ &get_table, &offset, &mode ]( this auto &&self, void *virtual_address, uint64_t level, pmlxt &pmlx_t ) -> uint64_t * {
    //     auto index { pmlx_t.get_virtual_index( (std::uint64_t)virtual_address ) };
    //     if ( !pmlx_t.flags_p( index ) ) {
    //         return nullptr;
    //     }
    //     if ( !( ( level - offset ) - 1 ) ) {
    //         auto entry = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
    //         return (uint64_t *)virtual_to_physical( entry );
    //     }
    //     else {
    //         auto entry             = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, Page::Type::P4Kib ) );
    //         get_table( level - 1 ) = entry;
    //         return self( virtual_address, level - 1, get_table( level - 1 ) );
    //     }
    // }( virtual_address, level, *this );
}
auto Paging::pmlxt::page_protect( bool flags ) -> void {
    auto cr0 = Driver::IO::read_cr0( );
    cr0.WP   = !flags;
    Driver::IO::write_cr0( cr0 );
};

auto Paging::pmlxt::activate( void ) -> void {
    if ( this->get_table( ) ) {
        Driver::IO::set_page_table( (uint64_t *)virtual_to_physical( this->get_table( ) ) );
    }
}
auto Paging::pmlxt::copy( pmlxt &from ) -> void {
    std::memset( this->pmlx_table, 0, PT_SIZE / 2 );
    std::memcpy( this->pmlx_table + 256, from.get_table( ) + 256, PT_SIZE / 2 );
}

Paging::pmlxt::pmlxt( void ) noexcept :
    can_allocate { true },
    pmlx_table { /* (uint64_t *)physical_to_virtual( allocator_traits< Page::allocator< Page::Type::P4Kib > >::allocate( table_allocator, 1 ) )*/ } {
}
Paging::pmlxt::~pmlxt( void ) noexcept {
    Page::allocator< Page::Type::P4Kib > table_allocator;
    if ( this->can_allocate ) {
        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( this->pmlx_table ), 1 );
    }
}

}     // namespace Memory