#pragma once
#include <atomic>
#include <concepts>
#include <utility>

namespace Task {

struct s_locks final : std::atomic_flag {
    std::atomic_uint64_t reference_count;

public:
    explicit s_locks( void ) noexcept {
    }
    explicit s_locks( bool locked ) noexcept {
        this->_M_i = locked;
    }
    virtual ~s_locks( void ) noexcept = default;

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
            __asm__ __volatile__( "pause\n\t" );
        }
    }
    auto locked( ) -> bool {
        return this->_M_i;
    }
};

inline s_locks kernel_thread_lock { };     // be provided for kernel thread
// user processes should create thier own locks.

template < typename T >
class spinlock final {
public:
    spinlock( s_locks &_m_lock = kernel_thread_lock ) :
        m_lock { _m_lock } {
    }
    template < typename F >
        requires std::invocable< F, T & >
    auto visit( F visitor ) {
        if ( !this->m_lock.reference_count ) {
            m_lock.acquire( );
        }
        this->m_lock.reference_count++;
        if constexpr ( std::is_invocable_r_v< void, F, T & > ) {
            visitor( this->value );
            this->m_lock.reference_count--;
            if ( !this->m_lock.reference_count ) {
                m_lock.release( );
            }
            return;
        }
        else {
            decltype( auto ) return_value = visitor( this->value );
            this->m_lock.reference_count--;
            if ( !this->m_lock.reference_count ) {
                m_lock.release( );
            }
            return return_value;
        }
        std::unreachable( );
    }

private:
private:
    T        value;
    s_locks &m_lock;
};

}     // namespace Task