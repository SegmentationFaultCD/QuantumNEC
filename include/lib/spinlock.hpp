#pragma once
#include <atomic>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Lib {
class spinlock :
    std::atomic_flag {
public:
    explicit spinlock( void ) noexcept {
    }
    explicit spinlock( IN bool locked ) noexcept {
        this->_M_i = locked;
    }
    virtual ~spinlock( void ) noexcept = default;

public:
    /**
     * @brief 释放锁
     */
    auto release( void ) {
        std::atomic_flag_clear_explicit( this, std::memory_order_release );
    }
    /**
     * @brief 获取锁
     */
    auto acquire( void ) {
        while ( std::atomic_flag_test_and_set_explicit( this, std::memory_order_acquire ) ) {
#if defined( __x86_64__ )
            __asm__ __volatile__( "pause\n\t" );
#elif defined( __aarch64__ ) || defined( __riscv )

#elif defined( __loongarch64 )

#endif
        }
    }
    auto locked( ) -> bool {
        return this->_M_i;
    }
};
}     // namespace QuantumNEC::Lib
