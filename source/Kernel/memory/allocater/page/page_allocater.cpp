#include "Kernel/print.hpp"
#include "Lib/Uefi.hpp"
#include <Kernel/memory/allocater/page/page_allocater.hpp>
#include <Kernel/memory/manager/page/page_header.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static uint64_t global_memory_address { };
    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_4K ] };
        using PH = __page_header< PAGE_4K, PAGE_2M, PAGE_2M >;
        using PHI = PH::__page_information;

        if ( __size < PH::page_descriptor_count ) {
            lock.acquire( );
            auto index = 0ul;
            auto bitmap_index = 0ul;
            auto node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = ( (PHI *)node[ i ].container )->bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );
            if ( node ) {
                auto &page_header = ( (PHI *)node->container )[ index ];
                page_header.bitmap->set( bitmap_index, __size );
                page_header.free_memory_page_count -= __size;
                if ( !page_header.free_memory_page_count ) {
                    page_header.flags.state = PH::ALL_FULL;
                }
                else {
                    page_header.flags.state = PH::NORMAL;
                }
                auto address = page_header.base_adderess + bitmap_index * this->__page_size< PAGE_4K >;
                std::memset( physical_to_virtual( address ), 0, __size * this->__page_size< PAGE_4K > );
                lock.release( );
                return (VOID *)address;
            }
            lock.release( );
        }
        // 头数
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;
        lock.acquire( );
        PH page_headers { group_header_count, { 0, -1 }, { 0, -1 } };
        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        std::memset( (VOID *)address, 0, __size * this->__page_size< PAGE_4K > );
        lock.release( );
        return (void *)address;
    }
    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_2M ] };
        using PH = __page_header< PAGE_2M, PAGE_2M, PAGE_1G >;
        using PHI = PH::__page_information;

        if ( __size < PH::page_descriptor_count ) {
            lock.acquire( );
            auto index = 0ul;
            auto bitmap_index = 0ul;

            auto node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = ( (PHI *)node[ i ].container )->bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );

            if ( node ) {
                auto &page_header = ( (PHI *)node->container )[ index ];
                page_header.bitmap->set( bitmap_index, __size );
                page_header.free_memory_page_count -= __size;
                if ( !page_header.free_memory_page_count ) {
                    page_header.flags.state = PH::ALL_FULL;
                }
                else {
                    page_header.flags.state = PH::NORMAL;
                }
                auto address = page_header.base_adderess + bitmap_index * this->__page_size< PAGE_2M >;
                std::memset( physical_to_virtual( address ), 0, __size * this->__page_size< PAGE_2M > );

                lock.release( );
                return (VOID *)address;
            }
            lock.release( );
        }
        // 头数
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;

        lock.acquire( );
        PH page_headers { group_header_count, { 0, -1 }, { 0, -1 } };

        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        std::memset( (VOID *)address, 0, __size * this->__page_size< PAGE_2M > );
        lock.release( );
        return (void *)address;
    }

    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size ) -> VOID * {
        auto &list { allocate_information_list[ MemoryPageType::PAGE_1G ] };
        using PH = __page_header< PAGE_1G, PAGE_2M, NONE >;
        using PHI = PH::__page_information;
        if ( __size < PH::page_descriptor_count ) {
            lock.acquire( );
            auto index = 0ul;
            auto bitmap_index = 0ul;
            auto node = list.traversal(
                [ &index, &bitmap_index ]( Lib::ListNode *node, uint64_t size ) -> BOOL {
                    for ( auto i = 0ul; i < PH::page_header_count; ++i ) {
                        if ( auto result = ( (PHI *)node[ i ].container )->bitmap->find< false >( size ); result.has_value( ) ) {
                            index = i;
                            bitmap_index = result.value( );
                            return TRUE;
                        }
                    }
                    return FALSE;
                },
                __size );
            if ( node ) {
                auto &page_header = ( (PHI *)node->container )[ index ];
                page_header.bitmap->set( bitmap_index, __size );
                page_header.free_memory_page_count -= __size;
                if ( !page_header.free_memory_page_count ) {
                    page_header.flags.state = PH::ALL_FULL;
                }
                else {
                    page_header.flags.state = PH::NORMAL;
                }
                auto address = page_header.base_adderess + bitmap_index * this->__page_size< PAGE_1G >;
                std::memset( physical_to_virtual( address ), 0, __size * this->__page_size< PAGE_1G > );
                lock.release( );
                return (VOID *)address;
            }
            lock.release( );
        }
        // 头数
        auto header_count = !__size % PH::page_descriptor_count ? __size / PH::page_descriptor_count : Lib::DIV_ROUND_UP( __size, PH::page_descriptor_count );
        // 组数
        auto group_header_count = header_count % PH::page_header_count ? header_count / PH::page_header_count + 1 : header_count / PH::page_header_count;
        lock.acquire( );
        PH page_headers { group_header_count, { 0, -1 }, { global_memory_address, 0 } };
        global_memory_address += header_count * this->__page_size< PAGE_1G >;
        // 开块
        page_headers.__allocate_headers( __size );
        // 拿第一个头的base
        auto address = std::get< PHI >( page_headers.get( 0 ) ).base_adderess;
        std::memset( (VOID *)address, 0, __size * this->__page_size< PAGE_1G > );
        lock.release( );
        return (void *)address;
    }
}