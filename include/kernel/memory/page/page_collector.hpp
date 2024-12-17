#pragma once
#include <kernel/memory/page/page_manager.hpp>
#include <lib/Uefi.hpp>
#include <lib/spin_lock.hpp>
namespace QuantumNEC::Kernel {
class PageCollector {
public:
    explicit PageCollector( void ) noexcept = default;
    virtual ~PageCollector( void ) noexcept = default;

public:
    template < MemoryPageType __type__ >
        requires( __type__ != MemoryPageType::NONE )
    auto free( IN void *__physical_address__, IN uint64_t __size__ ) -> void;
    // 这个操作会清空内存地址指向的空间中所有数据
};
template <>
auto PageCollector::free< MemoryPageType::PAGE_2M >( IN void *__physical_address__, IN uint64_t __size__ ) -> void;
template <>
auto PageCollector::free< MemoryPageType::PAGE_1G >( IN void *__physical_address__, IN uint64_t __size__ ) -> void;
template <>
auto PageCollector::free< MemoryPageType::PAGE_4K >( IN void *__physical_address__, IN uint64_t __size__ ) -> void;
}     // namespace QuantumNEC::Kernel