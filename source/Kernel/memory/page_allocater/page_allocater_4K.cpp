#include <Kernel/memory/memory.hpp>
#include <Kernel/memory/paging_map/ptv.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto Page4K::allocate( IN uint64_t size ) -> VOID * {
        lock.acquire( );
        if ( !size ) {
            lock.release( );
            // size空的就没有分配
            return NULL;
        }

        using PH = PageHeader< Page4K >;
        using PHI = PageHeader< Page4K >::PageInformation;

        if ( size < PH::MEMORY_PAGE_DESCRIPTOR ) {
            auto node = this->page_header_group_list.traversal(
                []( ListNode *node, auto size ) -> BOOL {
                    return ( PH::MEMORY_PAGE_DESCRIPTOR - ( (PHI *)node->container )->free_memory_page_count ) >= size;
                },
                size );
            if ( node ) {
                auto page_header = (PHI *)node->container;
                auto result = page_header->bitmap_->allocate( size );
                if ( result.has_value( ) ) {
                    page_header->free_memory_page_count -= size;
                    lock.release( );
                    return (VOID *)( page_header->map_base_adderess + result.value( ) * this->page_size );
                }
            }
        }
        Page2M p2m { };
        auto divisible = !( size % PH::MEMORY_PAGE_DESCRIPTOR );
        auto header_count = divisible ? size / PH::MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( size, PH::MEMORY_PAGE_DESCRIPTOR );

        PH page_headers { *this, header_count, MemoryPageType::PAGE_2M };
        page_headers.allocate< p2m.page_size >( &p2m );
        if ( divisible ) {
            page_headers.set_range_bit( );
            for ( auto i = 0ul; i < header_count; ++i ) {
                std::get< PHI >( page_headers.get( i ) ).free_memory_page_count = 0;
            }
        }
        else {
            if ( size < PH::MEMORY_PAGE_DESCRIPTOR ) {
                header_count--;
            }
            else {
                for ( auto i { 0ul }; i < header_count - 1; ++i ) {
                    page_headers.set_bit( i );
                    std::get< PHI >( page_headers.get( i ) ).free_memory_page_count = 0;
                }
            }
            auto &&Remainder = size % PH::MEMORY_PAGE_DESCRIPTOR;
            page_headers.set_bit( header_count, Remainder );
            std::get< PHI >( page_headers.get( header_count ) ).free_memory_page_count -= Remainder;
        }
        lock.release( );
        return (VOID *)std::get< PHI >( page_headers.get( 0 ) ).map_base_adderess;
    }
    auto Page4K::free( IN VOID * address, IN uint64_t size ) -> VOID {
        if ( !size || !address ) {
            return;
        }
        using PH = PageHeader< Page4K >;
        using PHI = PageHeader< Page4K >::PageInformation;
        auto base = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR );
        lock.acquire( );
        auto node = this->page_header_group_list.traversal(
            [ & ]( ListNode *node, uint64_t ) -> BOOL {
                return ( (PHI *)node->container )->map_base_adderess == base;
            },
            0 );

        if ( !node ) {
            lock.release( );
            return;
        }
        auto index = ( (uint64_t)address - base ) / this->page_size;
        auto header = (PHI *)node->container;
        if ( index + size > PH::PAGE_INFORMATION_HEADER_SIZE ) {
            // TODO :
            // N个头块之间的释放操作
            uint64_t header_count { }, end_remainder { };
            if ( auto tmp = index + size, end_remainder = tmp % PH::PAGE_INFORMATION_HEADER_SIZE; end_remainder ) {
                header_count = tmp / PH::PAGE_INFORMATION_HEADER_SIZE + 1;
            }
            else {
                header_count = tmp / PH::PAGE_INFORMATION_HEADER_SIZE;
            }
            // 头
            auto head_free_remainder = PH::PAGE_INFORMATION_HEADER_SIZE - index;
            header->free_memory_page_count += head_free_remainder;
            if ( header->free_memory_page_count == PH::PAGE_INFORMATION_HEADER_SIZE ) {
                header->flags.state = PH::ALL_FREE;
            }
            else {
                header->flags.state = PH::NORMAL;
            }
            header->bitmap_->free( index, head_free_remainder );
            std::memset( physical_to_virtual( address ), 0, head_free_remainder * this->page_size );
            auto node = header->group_node.next;
            for ( auto i = 0ul; i < header_count - 1; ++i ) {
                ( (PHI *)node->container )->free_memory_page_count = PH::MEMORY_PAGE_DESCRIPTOR;
                ( (PHI *)node->container )->flags.state = PH::ALL_FREE;
                ( (PHI *)node->container )->bitmap_->free( 0, PH::MEMORY_PAGE_DESCRIPTOR );
                std::memset( physical_to_virtual( ( (PHI *)node->container )->map_base_adderess ), 0, PH::MEMORY_PAGE_DESCRIPTOR * this->page_size );
                node = node->next;
            }
            auto end_header = ( (PHI *)node->next->container );
            end_header->flags.state = PH::NORMAL;
            end_header->free_memory_page_count += end_remainder;
            if ( end_header->free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                end_header->flags.state = PH::ALL_FREE;
            }
            end_header->bitmap_->free( 0, end_remainder );
            std::memset( physical_to_virtual( end_header->map_base_adderess ), 0, end_remainder * this->page_size );
            lock.release( );
            return;
        }
        header->bitmap_->free( index, size );
        header->free_memory_page_count += size;
        // 清空之前废弃的数据
        std::memset( physical_to_virtual( address ), 0, size * this->page_size );
        lock.release( );
        return;
    }
}