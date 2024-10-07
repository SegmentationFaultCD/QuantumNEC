#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/memory/manager/page/page_manager.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __page_allocater
    {
    public:
        template < MemoryPageType __type >
        constexpr static auto __page_size = ( [] consteval -> uint64_t {
            if constexpr ( __type == MemoryPageType::PAGE_2M ) {
                return 2_MB;
            }
            else if constexpr ( __type == MemoryPageType::PAGE_1G ) {
                return 1_GB;
            }
            else if constexpr ( __type == MemoryPageType::PAGE_4K ) {
                return 4_KB;
            }
            return 0ul;
        } )( );
        template < MemoryPageType __type >
        constexpr static auto __page_mask = ~( __page_size< __type > - 1 );
        template < MemoryPageType __type >
        constexpr static auto __page_aligned( auto address ) {
            return ( (uint64_t)address + __page_size< __type > - 1 ) & __page_mask< __type >;
        };

    public:
        explicit __page_allocater( VOID ) noexcept = default;
        virtual ~__page_allocater( VOID ) noexcept = default;

    public:
        // 分配器
        template < MemoryPageType __type >
            requires( __type != MemoryPageType::NONE )
        auto __allocate( IN uint64_t __size ) -> VOID *;
    };

    PUBLIC inline Lib::ListTable allocate_information_list[ 4 ] { };
    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size ) -> VOID *;
    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size ) -> VOID *;
    template <>
    auto __page_allocater::__allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size ) -> VOID *;
}