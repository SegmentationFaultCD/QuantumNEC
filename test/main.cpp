#include <concepts>
#include <cstdio>
#include <cstring>
template < typename CharT, typename... Args >
class basic_format_string {
private:
    const char *str;

public:
    template < typename T >
        requires std::convertible_to< const T &, const char * >
    consteval basic_format_string( const T &s ) :
        str { s } {
        std::size_t actual_number { };
        auto        length = std::strlen( str );
        for ( auto i = 0ul; i + 1 < length; ++i ) {
            if ( str[ i ] == '{' ) {
                for ( auto j = i + 1; j < length; ++j ) {
                    if ( str[ j ] == '}' ) {
                        actual_number++;
                        break;
                    }
                }
                // TODO Error handler
                while ( true );
            }
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
template < class... Args >
using format_string = basic_format_string< char, std::type_identity_t< Args >... >;
template < class Context >
class basic_format_args {
};
template < class OutputIt, class CharT >
class basic_format_context;

template < class OutputIt >
using format_context = basic_format_context< OutputIt, char >;
template < class OutputIt >
using format_args = basic_format_args< format_context< OutputIt > >;
template < typename... Args >
[[nodiscard]] auto vformat( const char *fmt, format_args< void > args ) -> const char * {
}
template < class OutputIt, typename... Args >
[[nodiscard]] auto vformat_to( OutputIt out, const char *fmt, format_args< OutputIt > args ) -> const char * {
}

auto make_format_args();

template < typename... Args >
[[nodiscard]] auto format( format_string< std::type_identity_t< Args >... > fmt, Args &&...args ) -> const char * {
    return vformat( fmt.get(), args... );
}
template < typename OutputIt, typename... Args >
[[nodiscard]] auto format_to( OutputIt out, format_string< std::type_identity_t< Args >... > fmt, Args &&...args ) -> OutputIt {
    return vformat_to( out, fmt, args... );
}
#include <format>
auto main( void ) -> int {
    std::format( "S{}", 1 );
}