#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/page/page_manager.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageCollector
    {
    public:
        explicit PageCollector( VOID ) noexcept = default;
        virtual ~PageCollector( VOID ) noexcept = default;

    public:
        template < MemoryPageType __type >
            requires( __type != MemoryPageType::NONE )
        auto free( IN VOID *__physical_address, IN uint64_t __size ) -> VOID;
        // 这个操作会清空内存地址指向的空间中所有数据
    };
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_2M >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_1G >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_4K >( IN VOID * __physical_address, IN uint64_t __size ) -> VOID;
}