#pragma once
#include <kernel/memory/page/page_manager.hpp>
#include <lib/Uefi.hpp>
#include <lib/spin_lock.hpp>
namespace QuantumNEC::Kernel {
class PageAllocater {
    friend PageManager;

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
    explicit PageAllocater( void ) noexcept = default;
    virtual ~PageAllocater( void ) noexcept = default;

public:
    // 分配器
    template < MemoryPageType __type__ >
        requires( __type__ != MemoryPageType::NONE )
    auto allocate( IN uint64_t __size__ ) -> void *;
    // 这个操作不会初始化分配的空间，需要手动清零
private:
    Lib::SpinLock lock { };
    // Global memory address is used by 1G allocater
    inline static uint64_t global_memory_address { };
};

template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size__ ) -> void *;
template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size__ ) -> void *;
template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size__ ) -> void *;
}     // namespace QuantumNEC::Kernel