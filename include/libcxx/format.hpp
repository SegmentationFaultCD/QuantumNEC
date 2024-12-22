#pragma once
#include <concepts>
#include <libcxx/cstring.hpp>
namespace std {
inline int16_t number_format { };
inline char    fmt_buffer[ 1024 * 64 ] { };
inline char    _buffer[ 1024 ] { };
struct format_context {
};
template < typename CharT, typename... Args >
class basic_format_string {
private:
    const char *str;

public:
    template < typename T >
        requires std::convertible_to< const T &, const char * >
    constexpr basic_format_string( const T &s ) :
        str { s } {
        std::size_t actual_number { };
        auto        length = std::strlen( str );
        for ( auto i = 0ul; i + 1 < length; ++i ) {
            if ( str[ i ] == '{' ) {
                for ( auto j = i + 1; j < length; ++j ) {
                    if ( str[ j ] == '}' ) {
                        actual_number++;
                        goto success;
                    }
                }
                // TODO Error handler
                while ( true );
            }
        success:
        }
        constexpr auto expectd_number { sizeof...( Args ) };
        if ( actual_number != expectd_number ) {
            // TODO Error handler
            while ( true );
        }
    }
    auto get( void ) const -> const char * {
        return str;
    }
};
template < class Context >
class basic_format_arg;
template < class... Args >
using format_string = basic_format_string< char, std::type_identity_t< Args >... >;

template < class OutputIt, class CharT >
class basic_format_context {
    std::basic_format_arg< basic_format_context > arg( std::size_t id ) const {
    }
};

template < class CharT >
class basic_format_parse_context;
template < class Context >
class format_arg {
    class handle {
    public:
        void format( std::basic_format_parse_context< typename Context::char_type > &parse_ctx,
                     Context                                                        &format_ctx ) const;

    private:
        void ( *func )( std::basic_format_parse_context< typename Context::char_type > &, Context &, const void * );
        const void *object;
    };

public:
    format_arg( ) noexcept {
    }

private:
    Context data;
};

template < class T, class CharT = char >
struct formatter {
    auto format( T ) -> char *;
};
template <>
struct formatter< char > {
    auto format( char value ) -> char *;
};
template <>
struct formatter< char && > {
    auto format( char &&value ) -> char *;
};
template <>
struct formatter< std::uint8_t && > {
    auto format( std::uint8_t &&value ) -> char *;
};
template <>
struct formatter< char * > {
    auto format( char *value ) -> char *;
};
template <>
struct formatter< const char * > {
    auto format( const char *value ) -> char *;
};
template <>
struct formatter< std::int32_t && > {
    auto format( std::int32_t &&value ) -> char *;
};
template <>
struct formatter< std::int16_t && > {
    auto format( std::int16_t &&value ) -> char *;
};
template <>
struct formatter< std::int64_t && > {
    auto format( std::int64_t &&value ) -> char *;
};
template <>
struct formatter< std::uint32_t && > {
    auto format( std::uint32_t &&value ) -> char *;
};
template <>
struct formatter< std::uint16_t && > {
    auto format( std::uint16_t &&value ) -> char *;
};
template <>
struct formatter< std::uint64_t && > {
    auto format( std::uint64_t &&value ) -> char *;
};
template <>
struct formatter< void * > {
    auto format( void *value ) -> char *;
};
template <>
struct formatter< double && > {
    auto format( double &&value ) -> char *;
};
template <>
struct formatter< std::int32_t > {
    auto format( std::int32_t value ) -> char *;
};
template <>
struct formatter< std::int16_t > {
    auto format( std::int16_t value ) -> char *;
};
template <>
struct formatter< std::int64_t > {
    auto format( std::int64_t value ) -> char *;
};
template <>
struct formatter< std::uint32_t > {
    auto format( std::uint32_t value ) -> char *;
};
template <>
struct formatter< std::uint16_t > {
    auto format( std::uint16_t value ) -> char *;
};
template <>
struct formatter< std::uint64_t > {
    auto format( std::uint64_t value ) -> char *;
};
template <>
struct formatter< std::uint8_t > {
    auto format( std::uint8_t value ) -> char *;
};
template <>
struct formatter< double > {
    auto format( double value ) -> char *;
};
auto _to_format( const char *&fmt, auto args ) {
    number_format = 10;
    while ( *fmt ) {
        if ( *fmt == '{' ) {
            if ( *( fmt + 1 ) == ':' ) {
                switch ( *( fmt + 2 ) ) {
                case 'x':
                    number_format = 16;
                    break;
                case 'o':
                    number_format = 8;
                    break;
                case 'i':
                case 'd':
                    number_format = 10;
                    break;
                case 'b':
                    number_format = 2;
                    break;
                default:
                    while ( true );
                }
                fmt += 4;
            }
            else {
                fmt += 2;
            }
            strcat( fmt_buffer, formatter< decltype( args ) >( ).format( args ) );
            return;
        }
        else {
            strcat( fmt_buffer, formatter< char >( ).format( *fmt++ ) );
        }
    }
}
template < typename... Args >
auto format( format_string< std::type_identity_t< Args >... > fmt, Args &&...args ) -> char * {
    memset( fmt_buffer, 0, strlen( fmt_buffer ) );
    auto _fmt = fmt.get( );
    ( _to_format( _fmt, args ), ... );
    strcat( fmt_buffer, _fmt );
    return fmt_buffer;
}
}     // namespace std