#pragma once
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>
#include <utility>
namespace Task {

struct s_locks final {
    std::atomic_flag lock;

public:
    explicit s_locks( void ) noexcept {
    }

    ~s_locks( void ) noexcept = default;

public:
    /**
     * @brief 释放锁
     */
    [[clang::always_inline]] auto release( void ) {
        std::atomic_flag_clear_explicit( &this->lock, std::memory_order::release );
    }
    /**
     * @brief 获取锁
     */
    [[clang::always_inline]] auto acquire( void ) {
        while ( std::atomic_flag_test_and_set_explicit( &this->lock, std::memory_order::acquire ) )
            __asm__ __volatile__( "pause" ::: "memory" );
    }
    auto locked( ) -> bool {
        return this->lock._M_i;
    }

    [[clang::always_inline]] auto try_lock( ) {
        return !std::atomic_flag_test_and_set_explicit( &this->lock, std::memory_order::acquire );
    }
};

inline s_locks kernel_thread_lock { };     // be provided for kernel thread
// user processes should create thier own locks.

}     // namespace Task