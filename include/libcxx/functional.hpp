#pragma once
#include <concepts>
#include <cstddef>
#include <libcxx/cstring.hpp>
#include <utility>
namespace std {
// 半成品

template < typename F >
class function;

template < typename R, class... Args >
class function< R( Args... ) > {
private:
    template < typename _R, typename... _Args >
    struct _ICallable_ {
        virtual auto invoke( _Args... args ) -> R = 0;
        virtual ~_ICallable_( ) {
        }
    };

    template < typename T, typename _R, typename... _Args >
    struct _Callable_ : _ICallable_< _R, _Args... > {
    private:
        T callable;

    public:
        _Callable_( T &&c ) :
            callable { std::move( c ) } {
        }

        auto invoke( _Args &&...args ) -> _R override {
            return callable( std::forward< _Args >( args )... );
        }
    };

public:
    function( ) noexcept {
    }
    function( std::nullptr_t ) noexcept {
    }
    function( [[maybe_unused]] const function &other ) noexcept {
    }
    function( [[maybe_unused]] function &&other ) noexcept {
    }

    template < class F >
        requires( !std::is_same_v< std::remove_cvref_t< F >, std::function< R( Args... ) > > ) && std::invocable< typename std::decay< F >::type, Args... >
    function( F &&fu ) :
        fp { new _Callable_< F, R, Args... > { std::forward< F >( fu ) } } {
    }
    ~function( ) {
        delete fp;
    }
    template < typename... _Args >
    auto operator( )( _Args &&...args ) const {
        this->fp->invoke( std::forward< _Args >( args )... );
    }
    auto operator=( [[maybe_unused]] const function &other ) {
    }

private:
    _ICallable_< R, Args... > *fp;
};
}     // namespace std