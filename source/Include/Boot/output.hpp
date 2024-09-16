#pragma once
#include <Boot/base.hpp>

namespace QuantumNEC::Boot {
inline auto base { 10 };

inline auto print( IN CONST uint64_t _value ) {
    if ( base == 16 ) {
        Print( (CHAR16 *)L"%lx", _value );
    }
    else if ( base == 10 ) {
        Print( (CHAR16 *)L"%lu", _value );
    }
    else if ( base == 8 ) {
        Print( (CHAR16 *)L"%lo", _value );
    }
}
inline auto print( IN CONST uchar_t *string ) {
    Print( (CHAR16 *)string );
}
inline auto print( IN CONST VOID *_value ) {
    Print( (CHAR16 *)L"%p", _value );
}
inline auto print( IN CONST uchar_t *fmt, IN CONST auto value ) {
    while ( *fmt ) {
        switch ( *fmt ) {
        case u'{':
            if ( *( fmt + 1 ) == u':' ) {
                switch ( *( fmt + 2 ) ) {
                case u'd':
                    base = 10;
                    break;
                case u'x':
                case u'X':
                    base = 16;
                    break;
                case u'o':
                    base = 8;
                    break;
                }
                fmt += 2;
            }
            print( value );
            fmt++;
            break;
        default:
            Print( (CHAR16 *)L"%c", (CHAR16)*fmt );
            break;
        }
        fmt++;
    }
}
inline auto print( IN CONST uchar_t *fmt, IN CONST auto value, CONST auto... args ) {
    while ( *fmt ) {
        switch ( *fmt ) {
        case u'{':
            if ( *( fmt + 1 ) == u':' ) {
                switch ( *( fmt + 2 ) ) {
                case u'd':
                    base = 10;
                    break;
                case u'x':
                case u'X':
                    base = 16;
                    break;
                case u'o':
                    base = 8;
                    break;
                }
                fmt += 2;
            }
            print( value );
            fmt += 2;
            print( fmt, args... );
            return;
        default:
            Print( (CHAR16 *)L"%c", (CHAR16)*fmt );
            break;
        }
        fmt++;
    }
}

}     // namespace QuantumNEC::Boot