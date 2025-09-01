#pragma once
#include <cstddef>
#include <fmt/core.h>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <lib/string.hpp>
#include <ranges>
#include <string>
namespace Library {

// 这个format极其简单，删除了format_to vformat_to_n 等等一系列函数, format_context等等辅助类
// 作者精神状态不好，写的很史
// 混合libfmt的终极答辩

inline auto stol( std::string_view s ) {
    std::uint64_t n = 0;
    std::uint64_t m = 1;
    for ( std::int64_t i = s.size( ) - 1; i >= 0; --i ) {
        n += ( s[ i ] - 48 ) * m;
        m *= 10;
    }
    return n;
}

enum class Align {
    left = '<',
    right = '>',
    center = '^'
};
enum class Sign {
    all = '+',
    only_neg = '-',
    space = ' '
};
using cxxstring = std::basic_string< char, std::char_traits< char >, Memory::KernelHeap::allocator< char > >;
template < typename T >
struct formatter {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( const T &, std::string_view fmt ) -> cxxstring { return { }; }
};
auto parse_sign( auto &&arg, Sign sign, cxxstring &ctx )
    requires std::three_way_comparable< decltype( arg ) >
{
    if ( auto result = arg <=> 0; result == std::strong_ordering::equal || result == std::strong_ordering::greater ) {
        if ( sign == Sign::all ) {
            ctx.insert( 0, "+" );
        }
        if ( sign == Sign::space ) {
            ctx.insert( 0, " " );
        }
        return;
    }
    else {
        ctx.insert( 0, "-" );
    }
}
inline auto parse_align( Align align, std::string_view data, cxxstring &ctx ) {
    using enum Align;
    switch ( align ) {
    case left:
        ctx.replace_with_range( ctx.begin( ), ctx.begin( ) + data.size( ) - 1, data );
        break;
    case right:
        ctx.replace_with_range( ctx.end( ) - data.size( ), ctx.end( ), data );
        break;
    case center:
        // TODO 中间处理
        break;
    }
}
auto parse_base( bool caps, std::int32_t base, auto &&arg ) {
    cxxstring data;

    const char *digits;
    if ( caps ) {
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
    else {
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    }
    do {
        data.insert( 0, sizeof( char ), digits[ arg % base ] );
        arg /= base;
    } while ( arg > 0 );
    return data;
}

inline auto parse_alternate_form( cxxstring &ctx, std::int32_t base ) {
    // TODO 前缀符
}
inline auto parse_leading_zeros( cxxstring &ctx ) {
    // TODO 先导0，align存在时忽略此处理
}

auto parse_format_spac( auto &&arg, std::string_view fmt ) -> cxxstring {
    auto i = 0;

    cxxstring weigh { };
    auto align = Align::right;
    auto fill = ' ';

    bool give_align = false;

    /* 先查一下是否存在fill-and-align(一定存在宽度)
     * 左移 <
     * 右移 >
     * 居中 ^
     */

    if ( ( fmt[ i ] < '0' || fmt[ i ] >= '9' )
         && fmt[ i ] != '^' && fmt[ i ] != '>' && fmt[ i ] != '<'
         && fmt[ i ] != 'x' ) {
        fill = fmt[ i ];
        i++;
    }

    if ( ( fmt[ i ] == '^' || fmt[ i ] == '<' || fmt[ i ] == '>' ) ) {
        align = (Align)fmt[ i ];
        give_align = true;
        i++;
    }
    auto sign = Sign::only_neg;

    switch ( fmt[ i ] ) {
    case ' ':     // 对于正值使用空格填充，负使用-
    case '+':     // 对于所有值均以对应符号填充
    case '-':     // 仅仅针对负值使用-填充，正值不予理会
        sign = static_cast< Sign >( fmt[ i ] );
        i++;
        break;
    }
    if ( fmt[ i ] >= '0' && fmt[ i ] <= '9' ) {
        // 懒得写范围直接c式循环
        for ( auto j = i; fmt[ j ] >= '0' && fmt[ j ] <= '9'; ++j ) {
            weigh.push_back( fmt[ j ] );
        }

        i += weigh.size( );
    }

    auto alternate_form = false;
    if ( fmt[ i ] == '#' ) {
        i++;
        alternate_form = true;
    }

    auto leading_zeros = false;
    if ( fmt[ i ] == '0' && !give_align ) {
        fill = '0';
        i++;
    }
    auto base = 10;
    auto caps = false;
    auto escape_appear = false;

    switch ( fmt[ i ] ) {
    case 'X':
        caps = true;
    case 'x':
        base = 16;
        break;
    case 'B':
        caps = true;
    case 'b':
        base = 2;
        break;
    case 'o':
        base = 8;
        break;
    case 'd':
        base = 10;
        break;
        // TODO
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
        break;
    }

    escape_appear = ( fmt[ i ] == '?' );
    cxxstring data;

    using T = std::remove_reference_t< decltype( arg ) >;
    if constexpr ( std::is_same_v< T, char > || std::is_same_v< T, const char * > ) {
        data += arg;     // todo 转义字符打印
        return data;
    }
    else if constexpr ( std::is_integral_v< T > || std::is_floating_point_v< T > ) {
        data.append( parse_base( caps, base, arg ) );
        parse_sign( arg, sign, data );
        if ( weigh == "" ) {
            return data;
        }
        auto fs = std::views::repeat( fill, stol( weigh.c_str( ) ) ) | std::ranges::to< cxxstring >( );
        parse_align( align, data, fs );
        return fs;
    }
    else {
        data.append( parse_base( true, 16, (unsigned long long)arg ) );
        return data;
    }
    return "";
}
template <>
struct formatter< long long unsigned int > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( long long unsigned int &arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
template <>
struct formatter< unsigned int > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( unsigned int &arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
template <>
struct formatter< int > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( int &arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};

template <>
struct formatter< long long int > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( long long arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};

template <>
struct formatter< const char * > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( const char *arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
template <>
struct formatter< char > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }
    auto format( char arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
template <>
struct formatter< const void * > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }

    auto format( const void *arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
template <>
struct formatter< void * > {
    constexpr auto parse( std::string_view fmt ) { return fmt; }

    auto format( void *arg, std::string_view ctx ) -> cxxstring {
        return parse_format_spac( arg, ctx );
    }
};
inline auto vformat( std::string_view fmt, fmt::format_args args ) -> cxxstring {
    fmt::format_parse_context f { fmt };
    auto index = 0;
    cxxstring formatted_string;
    for ( auto i = 0; i < fmt.size( ); ++i ) {
        if ( fmt[ i ] == '{' ) {
            if ( fmt[ i + 1 ] == '{' ) {
                formatted_string.push_back( '{' );
                i++;
                continue;
            }

            std::string_view format_spec = "";
            auto end = fmt.find_first_of( '}', i + 1 );
            auto spec_start = fmt.find_first_of( ':', i + 1 );
            if ( spec_start != fmt.npos ) {
                format_spec = fmt.substr( spec_start + 1, end - spec_start + 1 );
            }

            cxxstring arg_id;
            if ( spec_start != fmt.npos && fmt[ spec_start - 1 ] != '{' ) {
                arg_id = fmt.substr( i + 1, spec_start - i - 1 );
            }
            else {
                arg_id = std::to_string( index );
            }

            args.get( stol( arg_id.c_str( ) ) ).visit( [ & ]( auto data ) {
                using T = decltype( data );

                formatter< T > fmt;
                formatted_string.append_range( fmt.format( data, fmt.parse( format_spec ) ) );
            } );

            index++;
            i = end;
        }
        else if ( fmt[ i ] == '}' && fmt[ i + 1 ] == '}' ) {
            formatted_string.push_back( '}' );
            i++;
            continue;
            // }} 解析为 }
        }
        else {
            formatted_string.push_back( fmt[ i ] );
        }
    }
    formatted_string.push_back( '\0' );
    return formatted_string;
}

template < typename... Args >
auto format( fmt::format_string< Args... > fmt, Args... args ) -> cxxstring {
    return Library::vformat( fmt.get( ).data( ), fmt::make_format_args( args... ) );
}
}     // namespace Library