#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/memory/manager/page/page_manager.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __page_collector
    {
    public:
        explicit __page_collector( VOID ) noexcept = default;
        virtual ~__page_collector( VOID ) noexcept = default;

    public:
        template < MemoryPageType __type >
            requires( __type != MemoryPageType::NONE )
        auto __free( IN VOID *__physical_address, IN uint64_t __size ) -> VOID;
    };
    template <>
    auto __page_collector::__free< MemoryPageType::PAGE_2M >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
    template <>
    auto __page_collector::__free< MemoryPageType::PAGE_1G >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
    template <>
    auto __page_collector::__free< MemoryPageType::PAGE_4K >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
}