#include <kernel/memory/arch/memory_arch.hpp>
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
        using PH    = __page_header__< PAGE_2M, PAGE_1G >;
        using PHI   = PH::__helper__::__page_information__;
        auto &group = PH::__helper__::get_group( );
        lock.acquire( );

        auto node = group.search( PH::__helper__::get_keys( __physical_address__ ) );

        if ( node ) {
            auto zone = node->_data;
            if ( zone->owner ) {
                zone = zone->owner;
            }
            auto &&base_address = ( (uint64_t)__physical_address__ - zone->base_address );
            auto &&base_index   = ( base_address & PH::__helper__::__zone_memory_mask__( ) ) / PH::__helper__::__zone_min_memory__;                                               // 计算取得所在区域的header的编号
            auto &&index        = (base_address & PageAllocater::__page_mask__< PAGE_2M >) / PageAllocater::__page_size__< PAGE_2M > % PH::__helper__::page_descriptor_count;     // 取得处于所在header的bitmap中的编号

            PH       page_headers { zone };
            auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
            uint64_t end_remainder = index + __size__;
            if ( end_remainder <= PH::__helper__::page_descriptor_count ) {
                // 所释放的不超过一个header控制的大小
                header.bitmap->free( index, __size__ );
                header.free_memory_page_count += __size__;
                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }
                goto finish;
            }
            else {
                // 超过了，那就属于多个header处理范围
                end_remainder %= PH::__helper__::page_descriptor_count;
                // 头
                auto head_free_remainder = PH::__helper__::page_descriptor_count - index;     // 头的要释放根数量
                header.free_memory_page_count += head_free_remainder;

                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }

                header.bitmap->free( index, head_free_remainder );

                // 中
                auto page_header_count = ( __size__ - head_free_remainder - end_remainder ) % PH::__helper__::page_descriptor_count;
                for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                    auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                    middle_header.free_memory_page_count = PH::__helper__::page_descriptor_count;
                    middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                    middle_header.bitmap->free( 0, PH::__helper__::page_descriptor_count );
                }

                // 尾
                auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
                end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                end_header.free_memory_page_count += end_remainder;
                if ( end_header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                end_header.bitmap->free( 0, end_remainder );
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    if ( page_headers.get( i ).first.flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                        goto finish;
                    }
                }
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    auto node = &reinterpret_cast< PH::__helper__::__header__ * >( zone )[ i ].first.group_node;
                    group.destroy( node );
                }
                KHeapWalker { }.free( zone );
            }
        }

    finish:
        lock.release( );
        return;
    }
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_1G >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID {
        if ( !__size__ || !__physical_address__ ) {
            return;
        }
        using PH    = __page_header__< PAGE_1G, NONE >;
        using PHI   = PH::__helper__::__page_information__;
        auto &group = PH::__helper__::get_group( );
        lock.acquire( );

        auto node = group.search( PH::__helper__::get_keys( __physical_address__ ) );
        if ( node ) {
            auto zone = node->_data;
            if ( zone->owner ) {
                zone = zone->owner;
            }
            auto   &&base_address = ( (uint64_t)__physical_address__ - zone->base_address );
            auto   &&base_index   = ( base_address & PH::__helper__::__zone_memory_mask__( ) ) / PH::__helper__::__zone_min_memory__;                                               // 计算取得所在区域的header的编号
            auto   &&index        = (base_address & PageAllocater::__page_mask__< PAGE_1G >) / PageAllocater::__page_size__< PAGE_1G > % PH::__helper__::page_descriptor_count;     // 取得处于所在header的bitmap中的编号
            PH       page_headers { zone };
            auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
            uint64_t end_remainder = index + __size__;
            if ( end_remainder <= PH::__helper__::page_descriptor_count ) {
                // 所释放的不超过一个header控制的大小
                header.bitmap->free( index, __size__ );
                header.free_memory_page_count += __size__;
                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }
                goto finish;
            }
            else {
                // 超过了，那就属于多个header处理范围
                end_remainder %= PH::__helper__::page_descriptor_count;
                // 头
                auto head_free_remainder = PH::__helper__::page_descriptor_count - index;     // 头的要释放根数量
                header.free_memory_page_count += head_free_remainder;

                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }

                header.bitmap->free( index, head_free_remainder );

                // 中
                auto page_header_count = ( __size__ - head_free_remainder - end_remainder ) % PH::__helper__::page_descriptor_count;
                for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                    auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                    middle_header.free_memory_page_count = PH::__helper__::page_descriptor_count;
                    middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                    middle_header.bitmap->free( 0, PH::__helper__::page_descriptor_count );
                }

                // 尾
                auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
                end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                end_header.free_memory_page_count += end_remainder;
                if ( end_header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                end_header.bitmap->free( 0, end_remainder );
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    if ( page_headers.get( i ).first.flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                        goto finish;
                    }
                }
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    auto node = &reinterpret_cast< PH::__helper__::__header__ * >( zone )[ i ].first.group_node;
                    group.destroy( node );
                }
                KHeapWalker { }.free( zone );
            }
        }

    finish:
        lock.release( );
        return;
    }
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_4K >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID {
        if ( !__size__ || !__physical_address__ ) {
            return;
        }
        using PH    = __page_header__< PAGE_4K, PAGE_2M >;
        using PHI   = PH::__helper__::__page_information__;
        auto &group = PH::__helper__::get_group( );
        lock.acquire( );

        auto node = group.search( PH::__helper__::get_keys( __physical_address__ ) );
        if ( node ) {
            auto zone = node->_data;
            if ( zone->owner ) {
                zone = zone->owner;
            }
            auto   &&base_address = ( (uint64_t)__physical_address__ - zone->base_address );
            auto   &&base_index   = ( base_address & PH::__helper__::__zone_memory_mask__( ) ) / PH::__helper__::__zone_min_memory__;                                               // 计算取得所在区域的header的编号
            auto   &&index        = (base_address & PageAllocater::__page_mask__< PAGE_4K >) / PageAllocater::__page_size__< PAGE_4K > % PH::__helper__::page_descriptor_count;     // 取得处于所在header的bitmap中的编号
            PH       page_headers { zone };
            auto    &header        = std::get< PHI >( page_headers.get( base_index ) );
            uint64_t end_remainder = index + __size__;
            if ( end_remainder <= PH::__helper__::page_descriptor_count ) {
                // 所释放的不超过一个header控制的大小
                header.bitmap->free( index, __size__ );
                header.free_memory_page_count += __size__;
                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }
                goto finish;
            }
            else {
                // 超过了，那就属于多个header处理范围
                end_remainder %= PH::__helper__::page_descriptor_count;
                // 头
                auto head_free_remainder = PH::__helper__::page_descriptor_count - index;     // 头的要释放根数量
                header.free_memory_page_count += head_free_remainder;

                if ( header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                else {
                    header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }

                header.bitmap->free( index, head_free_remainder );

                // 中
                auto page_header_count = ( __size__ - head_free_remainder - end_remainder ) % PH::__helper__::page_descriptor_count;
                for ( auto i = base_index + 1; i < base_index + page_header_count; ++i ) {
                    auto &middle_header                  = std::get< PHI >( page_headers.get( i ) );
                    middle_header.free_memory_page_count = PH::__helper__::page_descriptor_count;
                    middle_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FREE;
                    middle_header.bitmap->free( 0, PH::__helper__::page_descriptor_count );
                }

                // 尾
                auto &end_header       = std::get< PHI >( page_headers.get( base_index + page_header_count + 1 ) );
                end_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                end_header.free_memory_page_count += end_remainder;
                if ( end_header.free_memory_page_count == PH::__helper__::page_descriptor_count ) {
                    end_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FREE;
                }
                end_header.bitmap->free( 0, end_remainder );
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    if ( page_headers.get( i ).first.flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                        goto finish;
                    }
                }
                for ( auto i = 0ul; i < zone->header_count; ++i ) {
                    auto node = &reinterpret_cast< PH::__helper__::__header__ * >( zone )[ i ].first.group_node;
                    group.destroy( node );
                }
                KHeapWalker { }.free( zone );
            }
        }

    finish:
        lock.release( );
        return;
    }
}