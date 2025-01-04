#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/memory/x86_64/paging/paging.hpp>
#include <kernel/memory/x86_64/paging/ptv.hpp>
#include <kernel/print.hpp>
#include <lib/shared_spinlock.hpp>
namespace QuantumNEC::Kernel::x86_64 {
pml5t pml5_t_buffer;
pml4t pml4_t_buffer;
Paging::Paging( void ) noexcept {
    if ( __config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
        this->pml5_t                = new ( &pml5_t_buffer ) pml5t { (pml5t::page_table_entry *)physical_to_virtual( CPU::get_page_table( ) ) };
        this->support_5level_paging = true;
        this->kernel_page_table     = pml5_t;
    }
    else {
        this->pml4_t                = new ( &pml4_t_buffer ) pml4t { (pml4t::page_table_entry *)physical_to_virtual( CPU::get_page_table( ) ) };
        this->support_5level_paging = false;
        this->kernel_page_table     = pml4_t;
    }

    this->kernel_page_table->page_protect( false );
}
PageAllocater allocater { };
uint64_t      address { };

using namespace std;
auto pmlxt::map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode ) -> void {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto level     = Paging::support_5level_paging ? 5 : 4;
    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto map_helper = [ &mode, &get_table, &physics_address, &virtual_address, flags ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {
        // First, get the next page table's entry index in current page table
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );

        // Second, if level satisfy our needs
        // Enter handler.
        if ( level == static_cast< uint64_t >( mode ) ) {
            if ( mode != PAGE_4K ) {
                // User need to map huge page
                auto check_next_table_under_the_old = [ get_table ]( this auto &self, uint64_t level, uint64_t index, pmlxt &pmlx_t ) -> void {
                    // Check whether there are other page tables that under the old page table is referenced by the old entry or not.

                    if ( pmlx_t.get_table( )[ index ]         // The entry isn't NULL,
                         && !pmlx_t.flags_ps_pat( index )     // PS bit isn't set, that means there is a page table that is referenced by this entry. (If the next rule 'level != 1' is true)
                         && level != 1 ) {
                        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, PAGE_4K ) );
                        for ( auto i = 0; i < 512; ++i ) {
                            self( level - 1, i, get_table( level ) );
                        }
                        // After we delete the all of page tables that under the old page table,
                        // We also ought to delete this old page table
                        PageWalker { }.free< MemoryPageType::PAGE_4K >( virtual_to_physical( get_table( level - 1 ).get_table( ) ), 1 );
                    }
                    else if ( level == 1 ) {
                        // Delete this page table
                        // Then return. Because there is no page table under the pml1t.
                        PageWalker { }.free< MemoryPageType::PAGE_4K >( virtual_to_physical( pmlx_t.get_table( ) ), 1 );
                    }
                    return;
                };
                check_next_table_under_the_old( level, index, pmlx_t );
            }
            // After checking, now set the entry.
            pmlx_t = { index,                          // the entry's index in the current page table
                       physics_address & ~0x7FFul,     // The low 12 bits is ignoreded, and the further handle is in the pmlx_t's operator= function.
                       flags | get_table( level ).is_huge( mode ),
                       mode };
            physics_address += get_table( level ).check_page_size( mode );
            virtual_address += get_table( level ).check_page_size( mode );
            return;
        }
        else if ( !pmlx_t.flags_p( index ) || pmlx_t.flags_ps_pat( index ) ) {
            // P bit isn't set or ps bit is set, that means the entry don't point to any page tables
            auto new_ = allocater.allocate< MemoryPageType::PAGE_4K >( 1 );
            std::memset( physical_to_virtual( new_ ), 0, pmlx_t.PT_SIZE );
            pmlx_t = {
                index,                                                   // the entry's index in the current page table
                ( reinterpret_cast< uint64_t >( new_ ) & ~0x7FFul ),     // The low 12 bits is ignoreded, and the further handle is in the pmlx_t's operator= function.
                flags,
                PAGE_4K     // use PAGE_4K mode to set Beacuse the entry points to a page table rather than a block of memory.
            };
        }

        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, PAGE_4K ) );     // get the next table's base
        self( level - 1, get_table( level - 1 ) );
    };

    while ( size-- ) {
        map_helper( level, *this );
    }
}
auto pmlxt::unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode ) -> void {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto level        = Paging::support_5level_paging ? 5 : 4;
    auto unmap_helper = [ &get_table, &virtual_address, &mode ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {     // 辅助函数
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );
        if ( !pmlx_t.flags_p( index ) ) {
            virtual_address += get_table( level ).check_page_size( mode );
            return;
        }

        else if ( level == static_cast< uint64_t >( mode ) ) {
            pmlx_t.set_p( index, 0 );
            CPU::invlpg( reinterpret_cast< void * >( virtual_address ) );
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

auto pmlxt::find_physcial_address( IN void *virtual_address, IN MemoryPageType mode ) -> void * {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };

    auto offset { uint64_t( mode ) - 1 };
    auto level       = Paging::support_5level_paging ? 5 : 4;
    auto find_helper = [ &get_table, &offset, &mode ]( this auto &self, IN void *virtual_address, IN uint64_t level, IN pmlxt &pmlx_t ) -> uint64_t * {
        auto index { pmlx_t.get_address_index_in( virtual_address ) };
        if ( !pmlx_t.flags_p( index ) ) {
            return nullptr;
        }
        if ( !( ( level - offset ) - 1 ) ) {
            auto entry = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
            return (uint64_t *)virtual_to_physical( entry );
        }
        else {
            auto entry             = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, MemoryPageType::PAGE_4K ) );
            get_table( level - 1 ) = entry;
            return self( virtual_address, level - 1, get_table( level - 1 ) );
        }
    };
    return find_helper( virtual_address, level, *this );
}
auto pmlxt::page_protect( IN bool flags ) -> void {
    auto cr0 = CPU::read_cr0( );
    if ( !flags ) {
        cr0.WP = 1;
        CPU::write_cr0( cr0 );
    }
    else {
        cr0.WP = 0;
        CPU::write_cr0( cr0 );
    }
};

auto pmlxt::activate( void ) -> void {
    if ( this->get_table( ) ) {
        CPU::set_page_table( (uint64_t *)virtual_to_physical( this->get_table( ) ) );
    }
}
auto pmlxt::copy( IN pmlxt &from ) -> void {
    // copy high 2048 size.
    std::memset( this->pmlx_table, 0, PT_SIZE / 2 );
    std::memcpy( this->pmlx_table + 256, from.get_table( ) + 256, PT_SIZE / 2 );
}
auto pmlxt::make( void ) -> pmlxt & {
    this->pmlx_table = (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) );
    std::memset( this->pmlx_table, 0, this->PT_SIZE );
    return *this;
}
pml1t::pml1t( void ) noexcept :
    pmlxt( (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) ) {
}
pml2t::pml2t( void ) noexcept :
    pmlxt( (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) ) {
}
pml3t::pml3t( void ) noexcept :
    pmlxt( (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) ) {
}
pml4t::pml4t( void ) noexcept :
    pmlxt( (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) ) {
}
pml5t::pml5t( void ) noexcept :
    pmlxt( (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) ) {
}
}     // namespace QuantumNEC::Kernel::x86_64
