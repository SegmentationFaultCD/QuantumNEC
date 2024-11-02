#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_2M >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID {
        if ( !__size__ || !__physical_address__ ) {
            return;
        }
        auto &list { allocate_information_list[ PAGE_2M ] };
        using PH  = __page_header__< PAGE_2M, PAGE_2M, PAGE_1G >;
        using PHI = PH::__page_information__;
        // 掩码方式取得所在组基地址
        auto base_address = (uint64_t)__physical_address__ & ~( PageAllocater::__page_size__< PAGE_2M > * PH::page_descriptor_count - 1 );
        lock.acquire( );
        auto node = list.traversal(
            []( Lib::ListNode *node, uint64_t base_address ) -> BOOL {
                return ( (PHI *)node->container )->base_adderess == base_address;
            },
            base_address );     // 找到属于的组
        if ( !node ) {
            lock.release( );
            return;
        }
        // 计算取得所在组的块的编号
        auto &&base_index = ( (uint64_t)__physical_address__ - base_address ) / ( PH::page_descriptor_count * PageAllocater::__page_size__< PAGE_2M > );
        // 取得处于所在组的块的bitmap中的编号
        auto &&index = (((uint64_t)__physical_address__ & PageAllocater::__page_mask__< PAGE_2M >) / PageAllocater::__page_size__< PAGE_2M >) % PH::page_descriptor_count;

        PH page_headers { (PHI *)node->container };

        auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
        uint64_t end_remainder = index + __size__;
        if ( end_remainder <= PH::page_descriptor_count ) {
            // 所释放的不超过一个大组控制的大小
            header.bitmap->free( index, __size__ );
            header.free_memory_page_count += __size__;
            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }
        else {
            // 超过了，那就属于多个组处理范围
            end_remainder %= PH::page_descriptor_count;
            // 头
            auto head_free_remainder = PH::page_descriptor_count - index;     // 头的要释放根数量
            header.free_memory_page_count += head_free_remainder;

            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            header.bitmap->free( index, head_free_remainder );

            // 中
            auto page_header_count = ( __size__ - head_free_remainder - end_remainder ) % PH::page_descriptor_count;
            for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                middle_header.free_memory_page_count = PH::page_descriptor_count;
                middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                middle_header.bitmap->free( 0, PH::page_descriptor_count );
                std::println< std::ostream::HeadLevel::DEBUG >( "{}", middle_header.bitmap->count( ) );
            }

            // 尾
            auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
            end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            end_header.free_memory_page_count += end_remainder;
            if ( end_header.free_memory_page_count == PH::page_descriptor_count ) {
                end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            end_header.bitmap->free( 0, end_remainder );
        }

        lock.release( );
    }
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_1G >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID {
        if ( !__size__ || !__physical_address__ ) {
            return;
        }
        auto &list { allocate_information_list[ PAGE_1G ] };
        using PH  = __page_header__< PAGE_1G, PAGE_2M, NONE >;
        using PHI = PH::__page_information__;
        // 掩码方式取得所在组基地址
        auto base_address = (uint64_t)__physical_address__ & ~( PageAllocater::__page_size__< PAGE_1G > * PH::page_descriptor_count - 1 );
        lock.acquire( );
        auto node = list.traversal(
            []( Lib::ListNode *node, uint64_t base_address ) -> BOOL {
                return ( (PHI *)node->container )->base_adderess == base_address;
            },
            base_address );     // 找到属于的组
        if ( !node ) {
            lock.release( );
            return;
        }
        // 计算取得所在组的块的编号
        auto &&base_index = ( ( (uint64_t)__physical_address__ - base_address ) / (PH::page_descriptor_count * PageAllocater::__page_size__< PAGE_1G >));
        // 取得处于所在组的块的bitmap中的编号
        auto &&index = (((uint64_t)__physical_address__ & PageAllocater::__page_mask__< PAGE_1G >) / PageAllocater::__page_size__< PAGE_1G >) % PH::page_descriptor_count;

        PH page_headers { (PHI *)node->container };

        auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
        uint64_t end_remainder = index + __size__;
        if ( end_remainder <= PH::page_descriptor_count ) {
            // 所释放的不超过一个大组控制的大小
            header.bitmap->free( index, __size__ );
            header.free_memory_page_count += __size__;
            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }
        else {
            // 超过了，那就属于多个组处理范围
            end_remainder %= PH::page_descriptor_count;
            // 头
            auto head_free_remainder = PH::page_descriptor_count - index;     // 头的要释放根数量
            header.free_memory_page_count += head_free_remainder;

            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            header.bitmap->free( index, head_free_remainder );

            // 中
            auto page_header_count = ( __size__ - head_free_remainder - end_remainder ) % PH::page_descriptor_count;
            for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                middle_header.free_memory_page_count = PH::page_descriptor_count;
                middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                middle_header.bitmap->free( 0, PH::page_descriptor_count );
                std::println< std::ostream::HeadLevel::DEBUG >( "{}", middle_header.bitmap->count( ) );
            }

            // 尾
            auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
            end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            end_header.free_memory_page_count += end_remainder;
            if ( end_header.free_memory_page_count == PH::page_descriptor_count ) {
                end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            end_header.bitmap->free( 0, end_remainder );
        }

        lock.release( );
    }
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_4K >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID {
        if ( !__size || !__physical_address ) {
            return;
        }
        auto &list { allocate_information_list[ PAGE_4K ] };
        using PH  = __page_header__< PAGE_4K, PAGE_2M, PAGE_2M >;
        using PHI = PH::__page_information__;
        // 掩码方式取得所在组基地址
        auto base_address = (uint64_t)__physical_address & ~( PageAllocater::__page_size__< PAGE_4K > * PH::page_descriptor_count - 1 );
        lock.acquire( );
        auto node = list.traversal(
            []( Lib::ListNode *node, uint64_t base_address ) -> BOOL {
                return ( (PHI *)node->container )->base_adderess == base_address;
            },
            base_address );     // 找到属于的组
        if ( !node ) {
            lock.release( );
            return;
        }
        // 计算取得所在组的块的编号
        auto &&base_index = ( ( (uint64_t)__physical_address - base_address ) / (PH::page_descriptor_count * PageAllocater::__page_size__< PAGE_4K >));
        // 取得处于所在组的块的bitmap中的编号
        auto &&index = (((uint64_t)__physical_address & PageAllocater::__page_mask__< PAGE_4K >) / PageAllocater::__page_size__< PAGE_4K >) % PH::page_descriptor_count;

        PH page_headers { (PHI *)node->container };

        auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
        uint64_t end_remainder = index + __size;
        if ( end_remainder <= PH::page_descriptor_count ) {
            // 所释放的不超过一个大组控制的大小
            header.bitmap->free( index, __size );
            header.free_memory_page_count += __size;
            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }
        else {
            // 超过了，那就属于多个组处理范围
            end_remainder %= PH::page_descriptor_count;
            // 头
            auto head_free_remainder = PH::page_descriptor_count - index;     // 头的要释放根数量
            header.free_memory_page_count += head_free_remainder;

            if ( header.free_memory_page_count == PH::page_descriptor_count ) {
                header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            else {
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            header.bitmap->free( index, head_free_remainder );

            // 中
            auto page_header_count = ( __size - head_free_remainder - end_remainder ) % PH::page_descriptor_count;
            for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                middle_header.free_memory_page_count = PH::page_descriptor_count;
                middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                middle_header.bitmap->free( 0, PH::page_descriptor_count );
                std::println< std::ostream::HeadLevel::DEBUG >( "{}", middle_header.bitmap->count( ) );
            }

            // 尾
            auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
            end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            end_header.free_memory_page_count += end_remainder;
            if ( end_header.free_memory_page_count == PH::page_descriptor_count ) {
                end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
            }
            end_header.bitmap->free( 0, end_remainder );
        }

        lock.release( );
    }
}