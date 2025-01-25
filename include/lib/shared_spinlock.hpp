#pragma once
#include <concepts>
#include <cstdint>
#include <lib/spinlock.hpp>
namespace QuantumNEC::Lib {
template < typename T >
    requires std::copy_constructible< T >
class shared_spinlock {
public:
    shared_spinlock( T &&_data ) :
        data_ { std::move( _data ) }, lock { }, reference_count { } {
    }
    shared_spinlock( ) :
        data_ { }, lock { }, reference_count { } {
    }
    using self = shared_spinlock< T >;

public:
    auto visit( auto visitor )
        requires std::invocable< decltype( visitor ), const shared_spinlock< T > & >
    {
        return ( (const self *)this )->visit( visitor );
    }
    auto visit( auto visitor ) const
        requires std::invocable< decltype( visitor ), const shared_spinlock< T > & >
    {
        if ( !this->reference_count ) {
#if defined( __x86_64__ )

#elif defined( __aarch64__ ) || defined( __riscv )

#elif defined( __loongarch64 )

#endif
            lock.acquire( );
        }
        this->reference_count++;

        constexpr auto have_return_value = !requires {{visitor( *this )}->std::same_as<void>; };

        if constexpr ( have_return_value ) {
            decltype( auto ) return_value = visitor( *this );
            this->reference_count--;
            if ( !this->reference_count ) {
                lock.release( );
#if defined( __x86_64__ )

#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
            }
            return return_value;
        }
        else {
            visitor( *this );
            this->reference_count--;
            if ( !this->reference_count ) {
                lock.release( );
#if defined( __x86_64__ )

#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
            }
        }
    }
    auto value( ) const -> T & {
        return data_;
    }
    auto value( ) -> T & {
        static_assert( false, "You shouldn't use the func." );
        return this->data_;
    }

private:
    mutable T             data_;
    mutable spinlock      lock;
    mutable std::uint64_t reference_count;
};

}     // namespace QuantumNEC::Lib