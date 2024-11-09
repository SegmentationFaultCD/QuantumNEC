#pragma once
#include <kernel/memory/page/page_manager.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageAllocater {
    public:
        template < MemoryPageType __type__ >
            requires( __type__ != NONE )
        constexpr static auto __page_size__ = ( [] consteval {
            if constexpr ( __type__ == MemoryPageType::PAGE_2M ) {
                return 2_MB;
            }
            else if constexpr ( __type__ == MemoryPageType::PAGE_1G ) {
                return 1_GB;
            }
            else {
                return 4_KB;
            }
        } )( );
        template < MemoryPageType __type__ >
            requires( __type__ != MemoryPageType::NONE )
        constexpr static auto __page_mask__ = ~( __page_size__< __type__ > - 1 );
        template < MemoryPageType __type__ >
            requires( __type__ != MemoryPageType::NONE )
        constexpr static auto __page_aligned__( auto &&address ) {
            return ( (uint64_t)address + __page_size__< __type__ > - 1 ) & __page_mask__< __type__ >;
        };
        template < MemoryPageType __type__ >
        constexpr static auto __page_base__( IN auto &&address ) {
            return (uint64_t)address & __page_mask__< __type__ >;
        }

    public:
        explicit PageAllocater( VOID ) noexcept = default;
        virtual ~PageAllocater( VOID ) noexcept = default;

    public:
        // 分配器
        template < MemoryPageType __type__ >
            requires( __type__ != MemoryPageType::NONE )
        auto allocate( IN uint64_t __size__ ) -> VOID *;
        // 这个操作不会初始化分配的空间，需要手动清零
    };

    PUBLIC inline Lib::ListTable allocate_information_list[ 4 ] { };
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size__ ) -> VOID *;
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size__ ) -> VOID *;
    template <>
    auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size__ ) -> VOID *;
}