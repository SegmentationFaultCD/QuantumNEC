#pragma once
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>
#include <utility>
namespace Task {

// 我不知道为什么特么标准库的atomic_flags老是炸
// 遂决定自己写一个自旋锁

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
};

inline s_locks kernel_thread_lock { };     // be provided for kernel thread
// user processes should create thier own locks.

template < typename T >
class spinlock final {
    std::uint64_t reference_count;

public:
    spinlock( T &&value, s_locks &_m_lock = kernel_thread_lock ) :
        m_lock { _m_lock }, value { std::move( value ) } {
        std::construct_at( &_m_lock );
    }
    template < typename F >
        requires std::invocable< F, T & >
    [[clang::always_inline]] auto visit( F visitor ) {
        if ( !this->reference_count ) {
            m_lock.acquire( );
        }
        this->reference_count++;
        if constexpr ( std::is_invocable_r_v< void, F, T & > ) {
            visitor( this->value );
            this->reference_count--;
            if ( !this->reference_count ) {
                m_lock.release( );
            }
            return;
        }
        else {
            decltype( auto ) return_value = visitor( this->value );
            this->reference_count--;
            if ( !this->reference_count ) {
                m_lock.release( );
            }
            return return_value;
        }
        std::unreachable( );
    }

private:
    T value;
    s_locks &m_lock;
};

}     // namespace Task