#pragma once
#include <kernel/memory/page/page_manager.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageCollector {
    public:
        explicit PageCollector( VOID ) noexcept = default;
        virtual ~PageCollector( VOID ) noexcept = default;

    public:
        template < MemoryPageType __type__ >
            requires( __type__ != MemoryPageType::NONE )
        auto free( IN VOID *__physical_addres__s, IN uint64_t __size__ ) -> VOID;
        // 这个操作会清空内存地址指向的空间中所有数据
    };
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_2M >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID;
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_1G >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID;
    template <>
    auto PageCollector::free< MemoryPageType::PAGE_4K >( IN VOID * __physical_address__, IN uint64_t __size__ ) -> VOID;
}