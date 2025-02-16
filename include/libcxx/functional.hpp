#pragma once
#include <concepts>
#include <cstddef>
#include <libcxx/cstring.hpp>
#include <utility>
namespace std {
// 半成品

template < typename R, class... Args >
class function {
public:
    function( ) noexcept {
    }
    function( std::nullptr_t ) noexcept {
    }
    function( const function &other ) {
        this->f.f = other.f.f;
    }
    function( function &&other ) {
    }

    template < class F >
        requires( !std::is_same_v< std::remove_cvref_t< F >, std::function< R( Args... ) > > ) && std::invocable< typename std::decay< F >::type, Args... >
    function( F &&fu ) :
        f { std::forward< F >( fu ) } {
    }
    ~function( ) {
    }
    template < typename... _Args >
    auto operator( )( _Args &&...args ) const {
        this->f.invoke( std::forward< _Args >( args )... );
    }
    auto operator=( const function &other ) {
        this->f.f = other.f.f;
    }

private:
    template < typename Function >
    struct _callable_ {
    public:
        _callable_( Function &&fu ) :
            f { fu } {
        }
        template < typename... _Args >
        auto invoke( _Args &&...args ) const -> R {
            return ( *f )( std::forward< _Args >( args )... );
        }
        Function *f;
    };

    _callable_< R( Args... ) > f;
};
}     // namespace std