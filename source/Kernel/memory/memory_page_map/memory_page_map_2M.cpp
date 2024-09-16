#include <Kernel/memory/memory_page_map/memory_map_2M.hpp>
#include <Kernel/memory/pmlxt/pmlxt.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/memory/memory.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto MemoryPageMap2M::allocate( IN uint64_t size ) -> VOID * {
        lock.acquire( );
        if ( !size ) {
            lock.release( );
            // size空的就没有分配
            return NULL;
        }

        using PH = PageHeader< MemoryPageMap2M >;
        using PHI = PageHeader< MemoryPageMap2M >::PageInformation;

        if ( size < PH::MEMORY_PAGE_DESCRIPTOR ) {
            auto node = PH::page_header_group_list.traversal(
                []( ListNode *node, auto size ) -> BOOL {
                    return ( PH::MEMORY_PAGE_DESCRIPTOR - std::get< PHI >( *( (PH::header_t *)node->container ) ).bitmap_->count( ) ) >= size;
                },
                size );
            if ( node ) {
                auto page_header = std::get< PHI >( *( (PH::header_t *)node->container ) );
                auto result = page_header.bitmap_->allocate( size );
                if ( result.has_value( ) ) {
                    lock.release( );
                    return (VOID *)( page_header.map_base_adderess + result.value( ) * this->page_size );
                }
            }
        }
        MemoryPageMap1G mpm1g { };
        auto divisible = !( size % PH::MEMORY_PAGE_DESCRIPTOR );
        auto header_count = divisible ? size / PH::MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( size, PH::MEMORY_PAGE_DESCRIPTOR );

        PH page_headers { *this, header_count, MemoryPageType::PAGE_2M };
        page_headers.allocate< mpm1g.page_size >( &mpm1g );
        if ( divisible ) {
            page_headers.set_range_bit( );
        }
        else { 
            if ( size < PH::MEMORY_PAGE_DESCRIPTOR ) {
                header_count--;
            }
            else {
                for ( auto i { 0ul }; i < header_count - 1; ++i ) {
                    page_headers.set_bit( i );
                }
            }
            page_headers.set_bit( header_count, size % PH::MEMORY_PAGE_DESCRIPTOR );
        }
        lock.release( );
        return (VOID *)std::get< PHI >( page_headers.get( 0 ) ).map_base_adderess;
    }
    auto MemoryPageMap2M::free( IN VOID * address, IN uint64_t size ) -> VOID {
        lock.acquire( );
        if ( !size || !address ) {
            lock.release( );
            return;
        }
        using PH = PageHeader< MemoryPageMap2M >;
        using PHI = PageHeader< MemoryPageMap2M >::PageInformation;
        auto base = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR );

        // auto node = memory_page_table_2M.traversal(
        //     [ & ]( ListNode *node, uint64_t ) -> BOOL {
        //         return ( (PageHandler *)node->container )->map_base_adderess == base;
        //     },
        //     0 );
        // if ( !node ) {
        //     lock.release( );
        //     return;
        // }
        // auto index = ( (uint64_t)address - base ) / PAGE_2M_SIZE;
        // auto handler = (PageHandler *)node->container;
        // auto status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        // for ( auto i { index }; i < index + size; ++i ) {
        //     handler->bitmap[ i ] = { i, 0 };
        // }

        // // 清空之前废弃的数据
        // memset( physical_to_virtual( address ), 0, size * PAGE_2M_SIZE );
        // Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
        lock.release( );
    }
    // auto MemoryPageMap2M::first_init( IN Lib::bitset< MEMORY_PAGE_DESCRIPTOR > & bitmap ) -> VOID {
    //     // STATIC auto flags { 0 };
    //     // if ( !flags ) {
    //     //     // 系统的0x000000开始给一块2m作为页头信息存储
    //     //     // 系统的0x200000开始给一块2m作为bitmap存储

    //     //     auto page_block = new ( physical_to_virtual( PAGE_2M_SIZE ) ) PageBlock { };
    //     //     auto handler = new ( physical_to_virtual( 0 ) ) PageHandler { page_block };

    //     //     bitmap[ 0 ] = { 0, BITMAP_MASK };
    //     //     bitmap[ 1 ] = { 1, BITMAP_MASK };

    //     //     handler->node.container = handler;
    //     //     memory_page_table_2M.push( handler->node );
    //     //     handler->map_base_adderess = 0;
    //     //     for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
    //     //         handler->bitmap[ i ] = bitmap[ i ];
    //     //     }

    //     //     flags++;
    //     //     // 这个函数只能用一次
    //     // }
    // }
}