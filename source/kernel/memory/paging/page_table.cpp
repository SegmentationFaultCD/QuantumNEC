#include <kernel/driver/cpu/io.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Memory {
Paging::pml5t pml5_t_buffer;
Paging::pml4t pml4_t_buffer;
Paging::Paging( void ) noexcept {
}
auto Paging::initialize( limine_paging_mode_response *pg ) -> void {
    pml5_t = &pml5_t_buffer;
    pml4_t = &pml4_t_buffer;
    if ( pg->mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
        std::construct_at( pml5_t, (pml5t::page_table_entry *)physical_to_virtual( Driver::IO::get_page_table( ) ) );
        support_5level_paging = true;
        kernel_page_table     = pml5_t;
    }
    else {
        std::construct_at( pml4_t, (pml4t::page_table_entry *)physical_to_virtual( Driver::IO::get_page_table( ) ) );
        support_5level_paging = false;
        kernel_page_table     = pml4_t;
    }

    kernel_page_table->page_protect( false );
}

inline uint64_t                             __address__ { };
static Page::allocator< Page::Type::P4Kib > table_allocator;
using namespace std;
auto Paging::pmlxt::map( uint64_t physics_address, uint64_t virtual_address, uint64_t size, uint64_t flags, Page::Type mode ) -> void {
    pml1t  pml1t { nullptr };
    pml2t  pml2t { nullptr };
    pml3t  pml3t { nullptr };
    pml4t  pml4t { nullptr };
    pml5t  pml5t { nullptr };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto level     = Paging::support_5level_paging ? 5 : 4;
    auto get_table = [ & ]( uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto map_helper = [ &mode, &get_table, &physics_address, &virtual_address, flags ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );

        using enum Page::Type;
        if ( level == std::to_underlying( mode ) ) {
            if ( mode != P4Kib ) {
                auto check_next_table_under_the_old = [ get_table ]( this auto &self, uint64_t level, uint64_t index, pmlxt &pmlx_t ) -> void {
                    if ( pmlx_t.get_table( )[ index ]
                         && !pmlx_t.flags_ps_pat( index )
                         && level != 1 ) {
                        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, P4Kib ) );
                        for ( auto i = 0; i < 512; ++i ) {
                            self( level - 1, i, get_table( level ) );
                        }

                        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( (uint64_t)get_table( level - 1 ).get_table( ) ), 1 );
                    }
                    else if ( level == 1 ) {
                        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( (uint64_t)pmlx_t.get_table( ) ), 1 );
                    }
                    return;
                };
                check_next_table_under_the_old( level, index, pmlx_t );
            }
            pmlx_t = { index,
                       physics_address & ~0x7FFul,
                       flags | get_table( level ).is_huge( mode ),
                       mode };
            physics_address += get_table( level ).check_page_size( mode );
            virtual_address += get_table( level ).check_page_size( mode );
            return;
        }
        else if ( !pmlx_t.flags_p( index ) || pmlx_t.flags_ps_pat( index ) ) {
            auto new_ = (uint64_t)allocator_traits< decltype( table_allocator ) >::allocate( table_allocator, 1 );
            std::memset( (void *)physical_to_virtual( new_ ), 0, pmlx_t.PT_SIZE );
            pmlx_t = {
                index,
                ( reinterpret_cast< uint64_t >( new_ ) & ~0x7FFul ),
                flags,
                P4Kib
            };
        }

        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, P4Kib ) );     // Get the next table's base
        self( level - 1, get_table( level - 1 ) );
    };

    while ( size-- ) {
        map_helper( level, *this );
    }
}
auto Paging::pmlxt::unmap( uint64_t virtual_address, std::size_t size, Page::Type mode ) -> void {
    pml1t  pml1t { nullptr };
    pml2t  pml2t { nullptr };
    pml3t  pml3t { nullptr };
    pml4t  pml4t { nullptr };
    pml5t  pml5t { nullptr };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto get_table = [ & ]( uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto level        = Paging::support_5level_paging ? 5 : 4;
    auto unmap_helper = [ &get_table, &virtual_address, &mode ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {     // 辅助函数
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );
        if ( !pmlx_t.flags_p( index ) ) {
            virtual_address += get_table( level ).check_page_size( mode );
            return;
        }
        else if ( level == std::to_underlying( mode ) ) {
            pmlx_t.set_p( index, 0 );
            if ( mode != Page::Type::P4Kib ) {
                auto check_next_table_under_the_old = [ get_table ]( this auto &self, uint64_t level, uint64_t index, pmlxt &pmlx_t ) -> void {
                    if ( pmlx_t.get_table( )[ index ]
                         && !pmlx_t.flags_ps_pat( index )
                         && level != 1 ) {
                        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, Page::Type::P4Kib ) );
                        for ( auto i = 0; i < 512; ++i ) {
                            self( level - 1, i, get_table( level ) );
                        }

                        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( (uint64_t)get_table( level - 1 ).get_table( ) ), 1 );
                    }
                    else if ( level == 1 ) {
                        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( (uint64_t)pmlx_t.get_table( ) ), 1 );
                    }
                    return;
                };
                check_next_table_under_the_old( level, index, pmlx_t );
            }
            // After all of these works, flush TLB.
            Driver::IO::invlpg( reinterpret_cast< void * >( virtual_address ) );
            virtual_address += get_table( level ).check_page_size( mode );
            return;
        }
        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
        self( level - 1, get_table( level - 1 ) );
    };
    while ( size-- ) {
        unmap_helper( level, *this );
    }
}

auto Paging::pmlxt::find_physcial_address( void *virtual_address, Page::Type mode ) -> void * {
    pml1t  pml1t { nullptr };
    pml2t  pml2t { nullptr };
    pml3t  pml3t { nullptr };
    pml4t  pml4t { nullptr };
    pml5t  pml5t { nullptr };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto get_table = [ & ]( uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };

    auto offset { std::to_underlying( mode ) - 1 };
    auto level       = Paging::support_5level_paging ? 5 : 4;
    auto find_helper = [ &get_table, &offset, &mode ]( this auto &self, void *virtual_address, uint64_t level, pmlxt &pmlx_t ) -> uint64_t * {
        auto index { pmlx_t.get_address_index_in( virtual_address ) };
        if ( !pmlx_t.flags_p( index ) ) {
            return nullptr;
        }
        if ( !( ( level - offset ) - 1 ) ) {
            auto entry = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
            return (uint64_t *)virtual_to_physical( entry );
        }
        else {
            auto entry             = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, Page::Type::P4Kib ) );
            get_table( level - 1 ) = entry;
            return self( virtual_address, level - 1, get_table( level - 1 ) );
        }
    };
    return find_helper( virtual_address, level, *this );
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
    pmlx_table { (uint64_t *)physical_to_virtual( allocator_traits< Page::allocator< Page::Type::P4Kib > >::allocate( table_allocator, 1 ) ) } {
}
Paging::pmlxt::~pmlxt( void ) noexcept {
    if ( this->can_allocate ) {
        allocator_traits< decltype( table_allocator ) >::deallocate( table_allocator, (void *)virtual_to_physical( this->pmlx_table ), 1 );
    }
}

}     // namespace Memory