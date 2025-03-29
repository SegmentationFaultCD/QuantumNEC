#pragma once
#include <concepts>
#include <cstdint>

#include <lib/spinlock.hpp>
#include <utility>
namespace QuantumNEC::Lib {

// smart_spinlock - 提供自动控制访问数据获取与释放锁的自旋锁包装器
// 防止上锁后，访问数据之后未释放的情况出现
// 想要访问这个自动锁管理的数据，只能使用观察器成员函数(visit)，严格保证不在上锁前，解锁后改动数据

template < typename T >
    requires std::copy_constructible< T > && std::move_constructible< T >
class smart_spinlock {
public:
    template < typename U >
        requires std::constructible_from< U, T >
    smart_spinlock( U &&_data ) :
        data_ { std::move( _data ) }, lock { }, reference_count { } {
    }

    smart_spinlock( ) :
        data_ { }, lock { }, reference_count { } {
    }

    using self = smart_spinlock< T >;

public:
    // visitor : 可调用类型，要求接受该包装器包装的类型引用，返回值任意
    // 调用visitor时，传入包装类型，可以访问与修改。
    template < typename F >
        requires std::invocable< F, T & >
    auto visit( F visitor ) {
        if ( !this->reference_count ) {
            lock.acquire( );
        }
        this->reference_count++;
        if constexpr ( std::is_same_v< std::invoke_result_t< F, T & >, void > ) {
            visitor( this->data_ );
            this->reference_count--;
            if ( !this->reference_count ) {
                lock.release( );
            }
        }
        else {
            decltype( auto ) return_value = visitor( this->data_ );
            this->reference_count--;
            if ( !this->reference_count ) {
                lock.release( );
            }
            return return_value;
            std::unreachable( );
        }
    }

    // 防止在上锁前，解锁后改动数据
    auto value( ) -> T & = delete;
    auto value( ) const -> const T & {
        return this->data_;
    };

private:
    mutable T             data_;
    mutable spinlock      lock;
    mutable std::uint64_t reference_count;
};

}     // namespace QuantumNEC::Lib