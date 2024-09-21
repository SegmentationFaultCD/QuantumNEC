#include "Kernel/memory/page_allocater/page_allocater_1G.hpp"
#include <Kernel/memory/memory.hpp>
#include <Kernel/memory/paging_map/ptv.hpp>
#include <Libcxx/string.hpp>

#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto PageAllocater1G::allocate( IN uint64_t size ) -> VOID * {
        if ( !size ) {
            // size空的就没有分配
            return NULL;
        }

        using PH = PageHeader< PageAllocater1G, PageAllocater2M >;
        using PHI = PH::PageInformation;

        if ( size <= PH::MEMORY_PAGE_DESCRIPTOR ) {
            lock.acquire( );
            // 最大分配数
            // 超过这个数无法保证内存连续，所以不允许分配超过这个数的
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
                    auto address = page_header->map_base_adderess + result.value( ) * this->page_size;
                    lock.release( );
                    return (VOID *)address;
                }
            }
            else {
                PageAllocater2M pa2m { };
                PH page_headers { *this, pa2m, MemoryPageType::PAGE_2M };
                auto info = std::get< PHI >( page_headers.get( ) );
                info.map_base_adderess = this->global_memory + this->page_size * PH::MEMORY_PAGE_DESCRIPTOR;
                this->global_memory += this->page_size * PH::MEMORY_PAGE_DESCRIPTOR;
                info.free_memory_page_count = 0;
                page_headers.set_range_bit( );
                lock.release( );
                return (void *)info.map_base_adderess;
            }
            lock.release( );
        }

        return NULL;
    }
    auto PageAllocater1G::free( IN VOID * address, IN uint64_t size ) -> VOID {
        if ( !size || !address ) {
            return;
        }
        using PH = PageHeader< PageAllocater1G, PageAllocater2M >;
        using PHI = PH::PageInformation;
        lock.acquire( );
        auto base = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR );
        0xffe000ul * 0x200000ul;

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
        // 清空之前废弃的数据
        header->bitmap_->free( index, size );
        header->free_memory_page_count += size;

        std::memset( physical_to_virtual( address ), 0, size * this->page_size );
        lock.release( );
        return;
    }
}