#include "Kernel/memory/page_allocater/page_allocater_4K.hpp"
#include <Kernel/memory/memory.hpp>
#include <Kernel/memory/paging_map/ptv.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto PageAllocater4K::allocate( IN uint64_t size ) -> VOID * {
        if ( !size ) {
            // size空的就没有分配
            return NULL;
        }

        using PH = PageHeader< PageAllocater4K, PageAllocater2M >;
        using PHI = PH::PageInformation;

        if ( size < PH::MEMORY_PAGE_DESCRIPTOR ) {
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
                    if ( !page_header->free_memory_page_count ) {
                        page_header->flags.state = PH::ALL_FULL;
                    }
                    else {
                        page_header->flags.state = PH::NORMAL;
                    }
                    auto address = page_header->map_base_adderess + result.value( ) * this->page_size;
                    lock.release( );
                    return (VOID *)address;
                }
            }
            lock.release( );
        }
        PageAllocater2M pa2m { };
        // 头数
        auto header_count = !size % PH::MEMORY_PAGE_DESCRIPTOR ? size / PH::MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( size, PH::MEMORY_PAGE_DESCRIPTOR );
        // 组数
        auto group_header_count = header_count % PH::MEMORY_PAGE_HEADER_COUNT ? header_count / PH::MEMORY_PAGE_HEADER_COUNT + 1 : header_count / PH::MEMORY_PAGE_HEADER_COUNT;
        lock.acquire( );
        PH page_headers { *this, pa2m, MemoryPageType::PAGE_2M, group_header_count };

        page_headers.allocate( pa2m, size );
        auto address = std::get< PHI >( page_headers.get( 0 ) ).map_base_adderess;
        lock.release( );
        return (void *)address;
    }
    auto PageAllocater4K::free( IN VOID * address, IN uint64_t size ) -> VOID {
        if ( !size || !address ) {
            return;
        }
        using PH = PageHeader< PageAllocater4K, PageAllocater2M >;
        using PHI = PH::PageInformation;
        lock.acquire( );
        auto group_base_address = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR * PH::MEMORY_PAGE_HEADER_COUNT - 1 );
        auto node = this->page_header_group_list.traversal(
            []( ListNode *node, uint64_t group_base_address ) -> BOOL {
                return ( (PHI *)node->container )->map_base_adderess == group_base_address;
            },
            group_base_address );     // 找到属于的组

        if ( !node ) {
            lock.release( );
            return;
        }

        auto base_map_address = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR - 1 );
        PH page_header {
            (PHI *)node->container
        };
        auto index = ( (uint64_t)address - group_base_address - base_map_address ) / this->page_size;

        auto &header = std::get< PHI >( page_header.get( index ) );
        if ( index + size <= PH::MEMORY_PAGE_DESCRIPTOR ) {
            header.bitmap_->free( index, size );
            header.free_memory_page_count += size;
            if ( header.free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                header.flags.state = PH::ALL_FREE;
            }
            else {
                header.flags.state = PH::NORMAL;
            }
            std::memset( physical_to_virtual( address ), 0, size * this->page_size );     // 清空之前废弃的数据
        }
        else {
            // TODO :
            // N个头块之间的释放操作
            uint64_t header_count { }, end_remainder { };
            if ( auto tmp = index + size, end_remainder = tmp % PH::MEMORY_PAGE_DESCRIPTOR; end_remainder ) {
                header_count = tmp / PH::MEMORY_PAGE_DESCRIPTOR + 1;
            }
            else {
                header_count = tmp / PH::MEMORY_PAGE_DESCRIPTOR;
            }
            // 头
            auto head_free_remainder = PH::MEMORY_PAGE_DESCRIPTOR - index;
            header.free_memory_page_count += head_free_remainder;
            if ( header.free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                header.flags.state = PH::ALL_FREE;
            }
            else {
                header.flags.state = PH::NORMAL;
            }
            header.bitmap_->free( index, head_free_remainder );
            std::memset( physical_to_virtual( address ), 0, head_free_remainder * this->page_size );
            // 中
            auto mid_header = header.next;

            for ( auto i = 0ul; i < header_count - 1; ++i ) {
                if ( !mid_header ) {
                    lock.release( );
                    return;     // 到时会有错误除处理
                }
                mid_header->free_memory_page_count = PH::MEMORY_PAGE_DESCRIPTOR;
                mid_header->flags.state = PH::ALL_FREE;
                mid_header->bitmap_->free( 0, PH::MEMORY_PAGE_DESCRIPTOR );
                std::memset( physical_to_virtual( mid_header->map_base_adderess ), 0, PH::MEMORY_PAGE_DESCRIPTOR * this->page_size );
                mid_header = mid_header->next;
            }
            // 尾
            auto end_header = mid_header;
            end_header->flags.state = PH::NORMAL;
            end_header->free_memory_page_count += end_remainder;
            if ( end_header->free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                end_header->flags.state = PH::ALL_FREE;
            }
            end_header->bitmap_->free( 0, end_remainder );
            std::memset( physical_to_virtual( end_header->map_base_adderess ), 0, end_remainder * this->page_size );
        }
        lock.release( );
    }
}