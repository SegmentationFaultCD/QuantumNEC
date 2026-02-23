#pragma once
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>
#include <utility>
namespace Task {

struct s_locks final {
    std::atomic_flag _lock;

public:
    explicit s_locks( void ) noexcept {
    }

    ~s_locks( void ) noexcept = default;

public:
    /**
     * @brief 释放锁
     */
    [[gnu::always_inline]] auto unlock( void ) {
        this->_lock.clear( std::memory_order::release );
    }
    /**
     * @brief 获取锁
     */
    [[gnu::always_inline]] auto lock( void ) {
        while ( this->_lock.test_and_set( std::memory_order::acquire ) );
    }

    [[gnu::always_inline]] auto try_lock( ) {
        return !this->_lock.test_and_set( std::memory_order::acquire );
    }
};
[[gnu::always_inline]] inline auto try_lock( s_locks *lock ) {
    return !std::atomic_flag_test_and_set( &lock->_lock );
}
inline s_locks kernel_thread_lock { };     // be provided for kernel thread
// user processes should create thier own locks.

}     // namespace Task