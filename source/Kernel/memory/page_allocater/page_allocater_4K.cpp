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

            PHI *find_node { };
            auto node = this->page_header_group_list.traversal(
                [ &find_node ]( ListNode *node, auto size ) -> BOOL {
                    for ( find_node = ( (PHI *)node->container ); find_node; find_node = find_node->next ) {
                        if ( ( PH::MEMORY_PAGE_DESCRIPTOR - find_node->free_memory_page_count ) >= size ) {
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                size );
            if ( node ) {
                auto result = find_node->bitmap_->allocate( size );
                if ( result.has_value( ) ) {
                    find_node->free_memory_page_count -= size;
                    if ( !find_node->free_memory_page_count ) {
                        find_node->flags.state = PH::ALL_FULL;
                    }
                    else {
                        find_node->flags.state = PH::NORMAL;
                    }
                    auto address = find_node->map_base_adderess + result.value( ) * this->page_size;
                    std::memset( (VOID *)address, 0, size );
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
        // 开块
        page_headers.allocate( pa2m, size );
        // 拿第一个头的map_base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).map_base_adderess;
        std::memset( (VOID *)address, 0, size );
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
        // 掩码方式取得所在组基地址
        auto group_base_address = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR * PH::MEMORY_PAGE_HEADER_COUNT - 1 );

        PHI *find_node { };
        auto node = this->page_header_group_list.traversal(
            [ &find_node ]( ListNode *node, uint64_t group_base_address ) -> BOOL {
                for ( find_node = ( (PHI *)node->container ); find_node; find_node = find_node->next ) {
                    if ( find_node->map_base_adderess == group_base_address ) {
                        return TRUE;
                    }
                }
                return FALSE;
            },
            group_base_address );     // 找到属于的组

        if ( !node ) {     // 没找到说明在释放不存在的页
            lock.release( );
            return;
        }
        // 掩码方式取得所在组基地址
        auto base_address = (uint64_t)address & ~( this->page_size * PH::MEMORY_PAGE_DESCRIPTOR - 1 );
        // 取得处于所在组中的头的bitmap中的编号
        auto index = ( (uint64_t)address - group_base_address - base_address ) / this->page_size;
        PH page_header { find_node };

        auto &header = std::get< PHI >( page_header.get( index ) );
        if ( index + size <= PH::MEMORY_PAGE_DESCRIPTOR ) {
            // 所释放的不超过一个大组控制的大小
            header.bitmap_->free( index, size );
            header.free_memory_page_count += size;
            if ( header.free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                header.flags.state = PH::ALL_FREE;
            }
            else {
                header.flags.state = PH::NORMAL;
            }
        }
        else {
            // 超过了，那就属于多个组处理范围
            uint64_t
                header_count { },      // 头信息块的数量
                end_remainder { };     // 结尾要释放根数量
            if ( auto tmp = index + size, end_remainder = tmp % PH::MEMORY_PAGE_DESCRIPTOR; end_remainder ) {
                header_count = tmp / PH::MEMORY_PAGE_DESCRIPTOR + 1;
            }
            else {
                header_count = tmp / PH::MEMORY_PAGE_DESCRIPTOR;
            }
            // 头
            auto head_free_remainder = PH::MEMORY_PAGE_DESCRIPTOR - ( index + 1 );     // 头的要释放根数量
            header.free_memory_page_count += head_free_remainder;
            if ( header.free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                header.flags.state = PH::ALL_FREE;
            }
            else {
                header.flags.state = PH::NORMAL;
            }
            header.bitmap_->free( index, head_free_remainder );

            // 中
            auto mid_header = header.next;     // 从头的下一节知道尾巴前一节
            while ( mid_header->next )         // 为假，说明已经达到了尾部，否则还在中间区域
            {
                mid_header->free_memory_page_count = PH::MEMORY_PAGE_DESCRIPTOR;
                mid_header->flags.state = PH::ALL_FREE;
                mid_header->bitmap_->free( 0, PH::MEMORY_PAGE_DESCRIPTOR );
                mid_header = mid_header->next;
            }
            // 尾
            auto end_header = mid_header;     // 此时mid_header已经指向结尾
            end_header->flags.state = PH::NORMAL;
            end_header->free_memory_page_count += end_remainder;
            if ( end_header->free_memory_page_count == PH::MEMORY_PAGE_DESCRIPTOR ) {
                end_header->flags.state = PH::ALL_FREE;
            }
            end_header->bitmap_->free( 0, end_remainder );
        }
        std::memset( physical_to_virtual( address ), 0, size * this->page_size );     // 清空之前废弃的数据
        lock.release( );
    }
}