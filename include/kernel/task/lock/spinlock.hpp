#pragma once
#include <atomic>
#include <concepts>
#include <utility>
namespace Task {

struct s_locks final : std::atomic_flag {
    std::atomic_uint64_t reference_count;
};

inline s_locks kernel_thread_lock { };     // be provided for kernel thread
// user processes should create thier own locks.

template < typename T >
class spinlock final {
public:
    spinlock( s_locks &_m_lock ) :
        m_lock { _m_lock } {
    }
    template < typename F >
        requires std::invocable< F, T & >
    auto visit( F visitor ) {
        if ( !this->m_lock.reference_count ) {
            this->lock( );
        }
        this->m_lock.reference_count++;
        if constexpr ( std::is_invocable_r_v< void, F, T & > ) {
            visitor( this->value );
            this->m_lock.reference_count--;
            if ( !this->m_lock.reference_count ) {
                this->unlock( );
            }
            return;
        }
        else {
            decltype( auto ) return_value = visitor( this->value );
            this->m_lock.reference_count--;
            if ( !this->m_lock.reference_count ) {
                this->unlock( );
            }
            return return_value;
        }
        std::unreachable( );
    }

private:
    auto lock( void ) {
        while ( std::atomic_flag_test_and_set_explicit( &m_lock, std::memory_order_acquire ) ) {
            __asm__ __volatile__( "pause\n\t" );
        }
    }
    auto unlock( void ) {
        std::atomic_flag_clear_explicit( &m_lock, std::memory_order_release );
    }

private:
    T        value;
    s_locks &m_lock;
};

}     // namespace Task