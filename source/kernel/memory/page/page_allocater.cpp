#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static uint64_t      global_memory_address { };
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size__ ) -> VOID * {
        using PH  = __page_header__< PAGE_4K, PAGE_2M >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );
        lock.acquire( );

        PHI *node { };
        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( auto result = std::get< PHI >( header[ i ] ).bitmap->find< false >( __size__ ); result.has_value( ) ) {
                            index        = i;
                            bitmap_index = result.value( );
                            node         = &zone;
                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
        else {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        continue;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
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
                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
    finish:
        if ( node ) {
            // 找到说明有符合的块可以使用

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

            std::memset( physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_4K > );
            lock.release( );
            return (VOID *)address;
        }
        // 先前开辟的全没符合要求
        // 那么就得开辟新块

        PH page_headers { header_count, { }, {} };

        // 开块
        page_headers.__allocate_headers__( __size__ );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_address;
        std::memset( physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_4K > );
        lock.release( );
        return (void *)address;
    }
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size__ ) -> VOID * {
        using PH  = __page_header__< PAGE_2M, PAGE_1G >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index        = 0ul;
        auto bitmap_index = 0ul;
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

        lock.acquire( );
        PHI *node { };
        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header

                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( auto result = std::get< PHI >( header[ i ] ).bitmap->find< false >( __size__ ); result.has_value( ) ) {
                            index        = i;
                            bitmap_index = result.value( );
                            node         = &zone;
                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
        else {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        continue;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
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

                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
    finish:
        if ( node ) {
            // 找到说明有符合的块可以使用

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
        auto header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

        lock.acquire( );
        PHI *node { };
        if ( __size__ < PH::__helper__::page_descriptor_count ) {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header
                    for ( auto i = 0ul; i < zone.header_count; ++i ) {
                        auto header = reinterpret_cast< PH::__helper__::__header__ * >( &zone );
                        if ( auto result = std::get< PHI >( header[ i ] ).bitmap->find< false >( __size__ ); result.has_value( ) ) {
                            index        = i;
                            bitmap_index = result.value( );
                            node         = &zone;
                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
        else {
            for ( auto &zone : group ) {
                if ( !zone.owner ) {     // owner为说明为此区域第一个header
                    if ( zone.header_count < header_count ) {
                        continue;     // 如果这个区域内header的数量甚至少于请求的header数那就直接下一个节点
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
                            goto finish;
                        }
                    }
                }
            }
            lock.release( );
            return NULL;
        }
    finish:
        if ( node ) {
            // 找到说明有符合的块可以使用

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

            std::memset( physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_1G > );
            lock.release( );
            return (VOID *)address;
        }

        // 先前开辟的全没符合要求
        // 那么就得开辟新块
        PH page_headers { header_count, { }, global_memory_address };
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