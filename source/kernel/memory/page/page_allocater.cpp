#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <lib/spin_lock.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
#include <libcxx/cstring.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static uint64_t global_memory_address { };
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_4K ] };
        using PH = __page_header< PAGE_4K, PAGE_2M, PAGE_2M >;
        using PHI = PH::__page_information;

        auto index = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        Lib::ListNode *node { };
        lock.acquire( );
        if ( __size < PH::page_descriptor_count ) {
            // 不超过一个header可控制的，那就找到1个内存大小足够的任意状态块
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );
        }
        else {
            // 超过一个header可控制的，那就找到几个连续的空闲块
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PH::__page_state::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( ( j == std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).header_count ) || std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PH::__page_state::ALL_FREE ) {
                                    goto out;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }
                out:
                    return FALSE;
                },
                header_count );
        }
        if ( node ) {
            // 找到说明有符合的块可以使用

            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] );
                page_header.bitmap->set( 0, PH::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state = PH::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PH::ALL_FULL;
            }
            else {
                page_header.flags.state = PH::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_adderess + bitmap_index * this->__page_size< PAGE_4K >;
            lock.release( );
            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;

        PH page_headers { group_header_count, { 0, -1 }, { 0, -1 } };

        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        lock.release( );
        return (void *)address;
    }
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_2M ] };
        using PH = __page_header< PAGE_2M, PAGE_2M, PAGE_1G >;
        using PHI = PH::__page_information;

        auto index = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        Lib::ListNode *node { };
        lock.acquire( );
        if ( __size < PH::page_descriptor_count ) {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );
        }
        else {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PH::__page_state::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( ( j == std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).header_count ) || std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PH::__page_state::ALL_FREE ) {
                                    goto out;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }
                out:
                    return FALSE;
                },
                header_count );
        }
        if ( node ) {
            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] );
                page_header.bitmap->set( 0, PH::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state = PH::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PH::ALL_FULL;
            }
            else {
                page_header.flags.state = PH::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_adderess + bitmap_index * this->__page_size< PAGE_2M >;
            lock.release( );
            return (void *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;
        PH page_headers { group_header_count, { 0, -1 }, { 0, -1 } };
        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        lock.release( );
        return (void *)address;
    }

    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_1G ] };
        using PH = __page_header< PAGE_1G, PAGE_2M, NONE >;
        using PHI = PH::__page_information;

        auto index = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        Lib::ListNode *node { };
        lock.acquire( );
        if ( __size < PH::page_descriptor_count ) {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );
        }
        else {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PH::__page_state::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( ( j == std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).header_count ) || std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PH::__page_state::ALL_FREE ) {
                                    goto out;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }
                out:
                    return FALSE;
                },
                header_count );
        }
        if ( node ) {
            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] );
                page_header.bitmap->set( 0, PH::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state = PH::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PH::ALL_FULL;
            }
            else {
                page_header.flags.state = PH::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_adderess + bitmap_index * this->__page_size< PAGE_1G >;
            lock.release( );
            return (void *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;
        PH page_headers { group_header_count, { 0, -1 }, { global_memory_address, 0 } };
        global_memory_address += header_count * this->__page_size< PAGE_1G >;
        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        lock.release( );
        return (void *)address;
    }
}