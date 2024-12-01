#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static uint64_t      global_memory_address { };
    int                         ui = 0;
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size__ ) -> VOID * {
        using PH  = __page_header__< PAGE_4K, PAGE_2M >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index        = 0ul;
        auto bitmap_index = 0ul;

        // The variable is not always used.
        // If size is less than page descriptor count of a header, the variable will not be used.
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

        PHI *node { };

        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            if ( group.empty( ) ) {
                goto finish;
            }

            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // zone.owner is null, that means it is the head of the zone.
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                        // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                        if ( auto page_information = std::get< PHI >( header[ i ] ); page_information.flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                            // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                            // Than, we should find a free bit from bitmap.
                            if ( auto result = page_information.bitmap->find< false >( __size__ ); result.has_value( ) ) {
                                index        = i;
                                bitmap_index = result.value( );
                                node         = &zone;
                                // Find the suitable zone to allocate, jump.
                                return TRUE;
                            }
                        }
                        else {
                            // switch to the next header.
                            continue;
                        }
                    }
                    // If you has arrived there, that means this zone of the group has no enough memory space to allocate.
                }
                return FALSE;
            } );
            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
        else {
            // If the requested size is more than one header can bear,
            // it means that we should to find a zone has a headers group and the headers'state is ALL_FREE.
            if ( group.empty( ) ) {
                goto finish;
            }
            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // owner为NULL说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        return FALSE;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
                    }
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( std::get< PHI >( header[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            auto pass = TRUE;
                            for ( auto j = i; j < header_count; ++j ) {
                                // 第二层，一直往前找
                                if ( std::get< PHI >( header[ j ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    pass = FALSE;
                                    break;
                                }
                            }
                            if ( !pass ) {
                                continue;
                            }
                            index = i;
                            node  = &zone;
                            // Find the suitable zone to allocate, jump.
                            return TRUE;
                        }
                    }
                }
                return FALSE;
            } );
            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
    finish:

        if ( node ) {
            // There are some suitable page headers searched.
            lock.acquire( );

            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ i ] );
                page_header.bitmap->set( 0, PH::__helper__::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size__ >= PH::__helper__::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size__ % PH::__helper__::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size__ );
            }
            auto address = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_4K >;

            lock.release( );
            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块

        PH page_headers { header_count, { }, {} };
        lock.acquire( );
        // 开块
        page_headers.__allocate_headers__( __size__ );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;
        return (void *)address;
    }
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size__ ) -> VOID * {
        using PH  = __page_header__< PAGE_2M, PAGE_1G >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        // The variable is not always used.
        // If size is less than page descriptor count of a header, the variable will not be used.
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

        PHI *node { };

        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            if ( group.empty( ) ) {
                goto finish;
            }

            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // zone.owner is null, that means it is the head of the zone.
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                        // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                        if ( auto page_information = std::get< PHI >( header[ i ] ); page_information.flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                            // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                            // Than, we should find a free bit from bitmap.
                            if ( auto result = page_information.bitmap->find< false >( __size__ ); result.has_value( ) ) {
                                index        = i;
                                bitmap_index = result.value( );
                                node         = &zone;

                                // Find the suitable zone to allocate, jump.
                                return TRUE;
                            }
                        }
                        else {
                            // switch to the next header.
                            continue;
                        }
                    }
                    // If you has arrived there, that means this zone of the group has no enough memory space to allocate.
                }
                return FALSE;
            } );

            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
        else {
            // If the requested size is more than one header can bear,
            // it means that we should to find a zone has a headers group and the headers'state is ALL_FREE.
            if ( group.empty( ) ) {
                goto finish;
            }
            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // owner为NULL说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        return FALSE;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
                    }
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( std::get< PHI >( header[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            auto pass = TRUE;
                            for ( auto j = i; j < header_count; ++j ) {
                                // 第二层，一直往前找
                                if ( std::get< PHI >( header[ j ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    pass = FALSE;
                                    break;
                                }
                            }
                            if ( !pass ) {
                                continue;
                            }
                            index = i;
                            node  = &zone;
                            // Find the suitable zone to allocate, jump.
                            return TRUE;
                        }
                    }
                }
                return FALSE;
            } );
            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
    finish:

        if ( node ) {
            // There are some suitable page headers searched.
            lock.acquire( );

            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ i ] );
                page_header.bitmap->set( 0, PH::__helper__::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size__ >= PH::__helper__::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size__ % PH::__helper__::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size__ );
            }
            auto address = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_2M >;

            lock.release( );
            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块

        PH page_headers { header_count, { }, {} };
        lock.acquire( );
        // 开块
        page_headers.__allocate_headers__( __size__ );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;

        lock.release( );
        return (void *)address;
    }

    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size__ ) -> VOID * {
        using PH  = __page_header__< PAGE_1G, NONE >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        // The variable is not always used.
        // If size is less than page descriptor count of a header, the variable will not be used.
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

        PHI *node { };

        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            if ( group.empty( ) ) {
                goto finish;
            }

            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // zone.owner is null, that means it is the head of the zone.
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                        // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                        if ( auto page_information = std::get< PHI >( header[ i ] ); page_information.flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                            // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                            // Than, we should find a free bit from bitmap.
                            if ( auto result = page_information.bitmap->find< false >( __size__ ); result.has_value( ) ) {
                                index        = i;
                                bitmap_index = result.value( );
                                node         = &zone;
                                // Find the suitable zone to allocate, jump.
                                return TRUE;
                            }
                        }
                        else {
                            // switch to the next header.
                            continue;
                        }
                    }
                    // If you has arrived there, that means this zone of the group has no enough memory space to allocate.
                }
                return FALSE;
            } );
            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
        else {
            // If the requested size is more than one header can bear,
            // it means that we should to find a zone has a headers group and the headers'state is ALL_FREE.
            if ( group.empty( ) ) {
                goto finish;
            }
            lock.acquire( );
            group.mid( [ & ]( PHI &zone ) -> BOOL {
                if ( !zone.owner ) {     // owner为NULL说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        return FALSE;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
                    }
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( std::get< PHI >( header[ i ] ).flags.state == PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            auto pass = TRUE;
                            for ( auto j = i; j < header_count; ++j ) {
                                // 第二层，一直往前找
                                if ( std::get< PHI >( header[ j ] ).flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    pass = FALSE;
                                    break;
                                }
                            }
                            if ( !pass ) {
                                continue;
                            }
                            index = i;
                            node  = &zone;
                            // Find the suitable zone to allocate, jump.
                            return TRUE;
                        }
                    }
                }
                return FALSE;
            } );
            lock.release( );
            // If you has arrived there, that means the group has no enough space to allocate.
            // So we should go to create a new zone.
        }
    finish:

        if ( node ) {
            // There are some suitable page headers searched.
            lock.acquire( );

            for ( auto i = index; i < index + header_count - 1; ++i ) {
                auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ i ] );
                page_header.bitmap->set( 0, PH::__helper__::page_descriptor_count );
                page_header.free_memory_page_count = 0;
                page_header.flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            auto &page_header = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index + header_count - 1 ] );
            page_header.free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;

            if ( !page_header.free_memory_page_count ) {
                page_header.flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                page_header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }

            if ( __size__ >= PH::__helper__::page_descriptor_count ) {
                page_header.bitmap->set( 0, __size__ % PH::__helper__::page_descriptor_count );
            }
            else {
                page_header.bitmap->set( bitmap_index, __size__ );
            }
            auto address = std::get< PHI >( reinterpret_cast< PH::__helper__::__header__ * >( node )[ index ] ).base_address + bitmap_index * this->__page_size__< PAGE_1G >;

            lock.release( );
            return (VOID *)address;
        }

        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        PH page_headers { header_count, { }, global_memory_address };
        lock.acquire( );
        global_memory_address += page_headers.all_memory_page_desvriptor_count * this->__page_size__< PAGE_1G >;
        // 开块
        page_headers.__allocate_headers__( __size__ );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;
        std::memset( physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_1G > );
        lock.release( );
        return (void *)address;
    }
}