#pragma once
#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>
namespace std {
template < class T, class E >
class expected;
template < class T, class E >
    requires std::is_void_v< T >
class expected< T, E >;
template < class E >
class unexpected {
public:
    constexpr unexpected( const unexpected & ) = default;
    constexpr unexpected( unexpected && )      = default;
    template < class Err = E >
        requires( !std::is_same_v< std::remove_cvref_t< Err >, unexpected > && !std::is_same_v< std::remove_cvref_t< Err >, std::in_place_t > && std::is_constructible_v< E, Err > )
    constexpr explicit unexpected( Err &&e ) :
        error_value { std::forward< Err >( e ) } {
    }
    template < class... Args >
        requires std::is_constructible_v< E, Args... >
    constexpr explicit unexpected( std::in_place_t, Args &&...args ) {
    }
    template < class U, class... Args >
        requires std::is_constructible_v< E, std::initializer_list< U > &, Args... >
    constexpr explicit unexpected( std::in_place_t,
                                   std::initializer_list< U > il, Args &&...args ) {
    }
    ~unexpected( void ) = default;
    constexpr const E &error( ) const & noexcept {
        return this->error_value;
    }
    constexpr E &error( ) & noexcept {
        return this->error_value;
    }
    constexpr const E &&error( ) const && noexcept {
        return this->error_value;
    }
    constexpr E &&error( ) && noexcept {
        return this->error_value;
    }

private:
    E error_value;
};
template < class E >
unexpected( E ) -> unexpected< E >;
template < class T, class E >
class expected {
    using value_type      = T;
    using error_type      = E;
    using unexpected_type = std::unexpected< E >;
    template < class U >
    using rebind = std::expected< U, error_type >;

    bool has_val;
    T    val;
    E    unex;

public:
    template < class U = T >
        requires( !std::is_same_v< std::remove_cvref_t< U >, std::in_place_t > && !std::is_same_v< std::expected< T, E >, std::remove_cvref_t< U > > && std::is_constructible_v< T, U > )
    constexpr explicit( !std::is_convertible_v< U, T > ) expected( U &&v ) :
        has_val { true }, val { v }, unex { } {
    }
    template < class U, class G >
        requires std::is_move_constructible_v< T > && std::is_move_constructible_v< E >
    constexpr expected( expected< U, G > &&other ) :
        has_val { other.has_val }, val { other.val }, unex { other.unex } {
    }

    template < class G >
    constexpr explicit( !std::is_convertible_v< const G &, E > )
        expected( std::unexpected< G > &&e ) :
        has_val { false }, val { }, unex { e.error( ) } {
    }

public:
    constexpr T &value( ) & {
        return this->val;
    }
    constexpr const T &value( ) const & {
        return this->val;
    }
    constexpr T &&value( ) && {
        return this->val;
    }
    constexpr const T &&value( ) const && {
        return this->val;
    }
    constexpr E &error( ) & {
        return this->unex;
    }
    constexpr const E &error( ) const & {
        return this->unex;
    }
    constexpr E &&error( ) && {
        return this->unex;
    }
    constexpr const E &&error( ) const && {
        return this->unex;
    }

public:
    constexpr explicit operator bool( ) const noexcept {
        return this->has_val;
    }
    constexpr auto has_value( ) const noexcept {
        return this->has_val;
    }
};
template < class T >
class expected< T, void > {
    using value_type = T;
    bool       has_val;
    value_type val;

public:
    template < class U = T >
        requires( !std::is_same_v< std::remove_cvref_t< U >, std::in_place_t > && !std::is_same_v< std::expected< T, void >, std::remove_cvref_t< U > > && std::is_constructible_v< T, U > )
    constexpr explicit( !std::is_convertible_v< U, T > ) expected( U &&v ) :
        val { v }, has_val { true } {
    }

    template < class U >
        requires std::is_move_constructible_v< T >
    constexpr expected( expected< U, void > &&other ) :
        val { other.val }, has_val { other.has_val } {
    }

    constexpr expected( void ) :
        val { }, has_val { false } {
    }

public:
    constexpr T &value( ) & {
        return this->val;
    }
    constexpr const T &value( ) const & {
        return this->val;
    }
    constexpr T &&value( ) && {
        return this->val;
    }
    constexpr const T &&value( ) const && {
        return this->val;
    }

public:
    constexpr explicit operator bool( ) const noexcept {
        return this->has_val;
    }
    constexpr auto has_value( ) const noexcept {
        return this->has_val;
    }
};
}     // namespace std