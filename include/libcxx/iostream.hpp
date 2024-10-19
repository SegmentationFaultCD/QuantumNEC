#pragma once
#include <lib/Uefi.hpp>
#include <libcxx/string.hpp>
#include <concepts>

PUBLIC namespace std {
    PUBLIC template < typename _TChar >
        requires std::same_as< _TChar, char_t > || std::same_as< _TChar, WCHAR > || std::same_as< _TChar, uchar_t >
    class basic_ostream;     // 共用
    struct ios
    {
        enum ios_base : char {
            dec = 0,     // 十进制
            oct = 1,     // 八进制
            hex = 2,     // 十六进制
            uppercase = 3,
            showbase = 4,
            showpos = 5
        };
        template < typename _CharT >
        struct s_stream
        {
            _CharT _s_w[ 0x1000 ] { };
        };
        explicit ios( VOID ) noexcept = default;
    };

    PUBLIC template <>
    class basic_ostream< char_t >
    {
    public:
        using C = char_t;

    public:
        enum class HeadLevel {
            INFO = 0,       // indigo
            DEBUG = 1,      // gray
            ERROR = 2,      // red
            SYSTEM = 3,     // gray blue
            USER = 4,       // white
            OK = 5,         // green
            START = 6,      // yellow
            WARNING = 7     // orange
        };
    };

    PUBLIC using ostream = basic_ostream< char_t >;
}
