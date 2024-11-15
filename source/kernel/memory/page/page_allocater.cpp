#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static uint64_t      global_memory_address { };
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_4K ] };
        using PH  = __page_header__< PAGE_4K, PAGE_2M >;
        using PHI = PH::__page_information__;

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );

        Lib::ListNode *node { };

        if ( __size < PH::page_descriptor_count ) {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index        = i;
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
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    break;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }

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
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_4K >;

            std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_4K > );

            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块

        PH page_headers { header_count, { }, {} };

        // 开块
        page_headers.__allocate_headers__( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;

        std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_2M > );

        return (void *)address;
    }
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_2M ] };
        using PH  = __page_header__< PAGE_2M, PAGE_1G >;
        using PHI = PH::__page_information__;

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );

        Lib::ListNode *node { };

        if ( __size < PH::page_descriptor_count ) {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index        = i;
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
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    break;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }

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
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_2M >;

            // std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_2M > );

            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块

        PH page_headers { header_count, { }, {} };

        // 开块
        page_headers.__allocate_headers__( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;
        // std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_2M > );

        return (void *)address;
    }

    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_1G ] };
        using PH  = __page_header__< PAGE_1G, NONE >;
        using PHI = PH::__page_information__;

        auto           index        = 0ul;
        auto           bitmap_index = 0ul;
        auto           header_count = Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        Lib::ListNode *node { };
        lock.acquire( );
        if ( __size < PH::page_descriptor_count ) {
            node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( auto result = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).bitmap->find< false >( size ); result.has_value( ) ) {
                            index        = i;
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
                    if ( !( (PH::header_t *)node )->first.header_count ) {
                        return FALSE;
                    }
                    for ( auto i = 0ul; i < ( (PH::header_t *)node )->first.header_count; ++i ) {
                        if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            for ( auto j = i; j < size; ++j ) {
                                if ( std::get< PHI >( ( (PH::header_t *)node->container )[ i ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    break;
                                }
                            }
                            index = i;
                            return TRUE;
                        }
                    }

                    return FALSE;
                },
                header_count );
        }
        if ( node ) {
            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ i ] );
                page_header.bitmap->set( 0, PH::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( ( (PH::header_t *)node->container )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size % PH::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size >= PH::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size % PH::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size );
            }
            auto address = std::get< PHI >( ( (PH::header_t *)node->container )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_1G >;
            std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_1G > );
            lock.release( );
            return (void *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        PH page_headers { header_count, { }, global_memory_address };
        global_memory_address += page_headers.all_memory_page_desvriptor_count * this->__page_size__< PAGE_1G >;
        // 开块
        page_headers.__allocate_headers__( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;
        std::memset( physical_to_virtual( address ), 0, __size * this->__page_size__< PAGE_1G > );
        lock.release( );
        return (void *)address;
    }
}