#include <Lib/Base/deflib.hpp>
#include <Lib/IO/Base/base_print.hpp>
#include <Lib/STL/string>
#include <Utils/utils.hpp>
PUBLIC namespace QuantumNEC::Utils {
    PUBLIC auto vsprintf( IN Lib::Types::Ptr< Lib::Types::char_t > buf, IN Lib::Types::Ptr< CONST Lib::Types::char_t > fmt, IN va_list args )->Lib::Types::int32_t {
        Lib::Types::Ptr< Lib::Types::char_t > str1 { }, str2 { };
        Lib::Types::int32_t flags { };
        Lib::Types::int32_t field_width { }, precision { }, len { }, i { }, qualifier { };
        for ( str1 = buf; *fmt; ++fmt ) {
            if ( *fmt != '%' )     // 如果不是'%'则认为是一个可显示字符
            {
                *str1++ = *fmt;
                continue;
            }
            flags = 0;
        repeat:
            ++fmt;
            switch ( *fmt )     // 如果是则判断后面跟的格式符
            {
            case '-':
                flags |= Lib::IO::LEFT;
                goto repeat;
            case '+':
                flags |= Lib::IO::PLUS;
                goto repeat;
            case ' ':
                flags |= Lib::IO::SPACE;
                goto repeat;
            case '#':
                flags |= Lib::IO::SPECIAL;
                goto repeat;
            case '0':
                flags |= Lib::IO::ZEROPAD;
                goto repeat;
            }
            // 提取出后续字符串里的数字并转换为数值, 用来表示数据区域的宽度
            field_width = -1;
            if ( Lib::Base::IS_DIGIT( *fmt ) ) {
                field_width = skip_atoi( &fmt );
            }
            // 如果不是数字而是'*', 那么数据区域将由可变参数提供
            else if ( *fmt == '*' ) {
                ++fmt;
                field_width = va_arg( args, Lib::Types::int32_t );
                if ( field_width < 0 ) {
                    field_width = -field_width;
                    flags |= Lib::IO::LEFT;
                }
            }
            // 提取出显示数据的精度
            precision = -1;
            if ( *fmt == '.' ) {
                ++fmt;
                if ( Lib::Base::IS_DIGIT( *fmt ) ) {
                    precision = skip_atoi( &fmt );
                }
                else if ( *fmt == '*' ) {
                    ++fmt;
                    precision = va_arg( args, Lib::Types::int32_t );
                    if ( precision < 0 ) {
                        precision = 0;
                    }
                }
            }
            // 检查显示数据的规格
            qualifier = -1;
            if ( *fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z' ) {
                qualifier = *fmt;
                ++fmt;
            }
            // 后辍
            switch ( *fmt ) {
            case 'c':
                if ( qualifier == 'l' ) {
                }
                else {
                    if ( !( flags & Lib::IO::LEFT ) ) {
                        while ( --field_width > 0 ) {
                            *( ++str1 ) = ' ';
                        }
                    }
                    *str1++ = (Lib::Types::char_t)va_arg( args, Lib::Types::int32_t );
                    while ( --field_width > 0 ) {
                        *str1++ = ' ';
                    }
                    break;
                }
                break;
            case 's':
                str2 = va_arg( args, Lib::Types::Ptr< Lib::Types::char_t > );
                if ( !str2 ) {
                    str2 = 0;
                }
                len = QuantumNEC::Lib::STL::strlen( str2 );
                if ( precision < 0 ) {
                    precision = len;
                }
                else if ( len > precision ) {
                    len = precision;
                }
                if ( !( flags & Lib::IO::LEFT ) ) {
                    while ( len < --field_width ) {
                        *str1++ = ' ';
                    }
                }
                for ( i = 0; i < len; ++i ) {
                    *str1++ = *str2++;
                }
                while ( len < --field_width ) {
                    *str1++ = ' ';
                }
                break;
            case 'o':
                if ( qualifier == 'l' ) {
                    str1 = number( str1, va_arg( args, Lib::Types::uint64_t ), 8,
                                   field_width, precision, flags );
                }
                else {
                    str1 = number( str1, va_arg( args, Lib::Types::uint32_t ), 8,
                                   field_width, precision, flags );
                }
                break;
            case 'p':
                if ( field_width == -1 ) {
                    field_width = 2 * sizeof( VOID near * );
                    flags |= Lib::IO::ZEROPAD;
                }

                str1 = number( str1,
                               (Lib::Types::uint64_t)va_arg( args, VOID near * ),
                               16, field_width, precision, flags );
                break;
            case 'x':
                flags |= static_cast< Lib::Types::int32_t >( Lib::IO::SMALL );
                goto NextX;
            case 'X':
            NextX:     // 只是为了回避编译器警告"this statement may fall
                       // through",下面也是
                if ( qualifier == 'l' ) {
                    str1 = number( str1, va_arg( args, Lib::Types::uint64_t ), 16,
                                   field_width, precision, flags );
                }
                else {
                    str1 = number( str1, va_arg( args, Lib::Types::uint32_t ), 16,
                                   field_width, precision, flags );
                }
                break;
            case 'd':
            case 'i':
                flags |= Lib::IO::SIGN;
                goto NextU;
            case 'u':
            NextU:
                if ( qualifier == 'l' ) {
                    str1 = number( str1, va_arg( args, Lib::Types::uint64_t ), 10,
                                   field_width, precision, flags );
                }
                else {
                    str1 = number( str1, va_arg( args, Lib::Types::uint32_t ), 10,
                                   field_width, precision, flags );
                }
                break;
            case 'b':
                if ( qualifier == 'l' ) {
                    str1 = number( str1, va_arg( args, Lib::Types::uint64_t ), 2,
                                   field_width, precision, flags );
                }
                else {
                    str1 = number( str1, va_arg( args, Lib::Types::uint32_t ), 2,
                                   field_width, precision, flags );
                }
                break;
            case 'n':
                if ( qualifier == 'l' ) {
                    Lib::Types::Ptr< Lib::Types::int64_t > IP = va_arg( args, Lib::Types::Ptr< Lib::Types::int64_t > );
                    *IP = ( str1 - buf );
                }
                else {
                    Lib::Types::Ptr< Lib::Types::int32_t > IP = va_arg( args, Lib::Types::Ptr< Lib::Types::int32_t > );
                    *IP = ( str1 - buf );
                }
                break;
            case 'f':
                if ( qualifier == 'l' ) {
                }
                else {
                }
                break;
            case '%':
                *str1++ = '%';
                break;
            default:
                *str1++ = '%';
                if ( *fmt ) {
                    *str1++ = *fmt;
                }
                else {
                    fmt--;
                }
                break;
            }
        }
        *str1 = '\0';
        return str1 - buf;
    }
    PUBLIC auto skip_atoi( IN CONST Lib::Types::Ptr< Lib::Types::Ptr< CONST Lib::Types::char_t > > str )->Lib::Types::size_t {
        Lib::Types::int32_t i { };
        while ( Lib::Base::IS_DIGIT( **str ) ) {
            i = i * 10 + *( ( *str )++ ) - '0';
        }
        return i;
    }

    PUBLIC auto number(
        IN OUT Lib::Types::Ptr< Lib::Types::char_t > str,
        IN Lib::Types::int64_t num, IN Lib::Types::int32_t base,
        IN Lib::Types::int32_t size,
        IN Lib::Types::int32_t precision,
        IN Lib::Types::int32_t type )
        ->Lib::Types::Ptr< Lib::Types::char_t > {
        Lib::Types::char_t ch { }, sign { }, tmp[ 50 ] { };
        Lib::Types::Ptr< CONST Lib::Types::char_t > digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        Lib::Types::int32_t i;

        if ( type & Lib::IO::SMALL )
            digits = "0123456789abcdefghijklmnopqrstuvwxyz";
        if ( type & Lib::IO::LEFT )
            type &= ~Lib::IO::ZEROPAD;
        if ( base < 2 || base > 36 )
            return 0;
        ch = ( type & Lib::IO::ZEROPAD ) ? '0' : ' ';
        sign = 0;
        if ( type & Lib::IO::SIGN && num < 0 ) {
            sign = '-';
            num = -num;
        }
        else {
            sign = ( type & Lib::IO::PLUS ) ? '+' : ( ( type & Lib::IO::SPACE ) ? ' ' : 0 );
        }
        if ( sign ) {
            size--;
        }
        if ( type & Lib::IO::SPECIAL ) {
            if ( base == 16 ) {
                size -= 2;
            }
            else {
                if ( base == 8 ) {
                    size--;
                }
            }
        }
        i = 0;
        if ( !num ) {
            tmp[ i++ ] = '0';
        }
        else {
            while ( num ) {
                tmp[ i++ ] = digits[ DO_DIV( num, base ) ];
            }
        }
        if ( i > precision ) {
            precision = i;
        }
        size -= precision;
        if ( !( type & ( Lib::IO::ZEROPAD + Lib::IO::LEFT ) ) ) {
            while ( size-- > 0 ) {
                *str++ = ' ';
            }
        }
        if ( sign ) {
            *str++ = sign;
        }
        if ( type & Lib::IO::SPECIAL ) {
            if ( base == 8 ) {
                *str++ = '0';
            }
            else if ( base == 16 ) {
                *str++ = '0';
                *str++ = digits[ 33 ];
            }
        }
        if ( !( type & Lib::IO::LEFT ) ) {
            while ( size-- > 0 ) {
                *str++ = ch;
            }
        }

        while ( i < precision-- ) {
            *str++ = '0';
        }
        while ( i-- > 0 ) {
            *str++ = tmp[ i ];
        }
        while ( size-- > 0 ) {
            *str++ = ' ';
        }
        return str;
    }
}