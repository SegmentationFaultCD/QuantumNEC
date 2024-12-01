#include <libc/ctype.h>
#include <libc/stdio.h>
#include <stdint.h>
#define ZEROPAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define LEFT 16
#define SPECIAL 32
#define SMALL 64

static size_t skip_atoi( const char **str ) {
    int32_t i { };
    while ( is_digit( **str ) ) {
        i = i * 10 + *( ( *str )++ ) - '0';
    }
    return i;
}
#define do_div( n, base ) ( {                                 \
    unsigned long __lower;                                    \
    unsigned long __high = n >> 32;                           \
    __lower              = (unsigned long)n;                  \
    __lower              = ( __lower % (unsigned long)base ); \
    n                    = ( __high ? __high : __lower );     \
    __lower;                                                  \
} )

static char *number(
    char   *str,
    int64_t num, int32_t base,
    int32_t size,
    int32_t precision,
    int32_t type ) {
    char        ch { }, sign { }, tmp[ 50 ] { };
    const char *digits { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    int32_t     i;
    if ( type & SMALL )
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if ( type & LEFT )
        type &= ~ZEROPAD;
    if ( base < 2 || base > 36 )
        return 0;
    ch   = ( type & ZEROPAD ) ? '0' : ' ';
    sign = 0;
    if ( type & SIGN && num < 0 ) {
        sign = '-';
        num  = -num;
    }
    else {
        sign = ( type & PLUS ) ? '+' : ( ( type & SPACE ) ? ' ' : 0 );
    }
    if ( sign ) {
        size--;
    }
    if ( type & SPECIAL ) {
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
            tmp[ i++ ] = digits[ do_div( num, base ) ];
        }
    }
    if ( i > precision ) {
        precision = i;
    }
    size -= precision;
    if ( !( type & ( ZEROPAD + LEFT ) ) ) {
        while ( size-- > 0 ) {
            *str++ = ' ';
        }
    }
    if ( sign ) {
        *str++ = sign;
    }
    if ( type & SPECIAL ) {
        if ( base == 8 ) {
            *str++ = '0';
        }
        else if ( base == 16 ) {
            *str++ = '0';
            *str++ = digits[ 33 ];
        }
    }
    if ( !( type & LEFT ) ) {
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
extern "C" int vsprintf( char *buf, const char *fmt, va_list args ) {
    char   *str1 { }, *str2 { };
    int32_t flags { };
    int32_t field_width { }, precision { }, len { }, i { }, qualifier { };
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
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        // 提取出后续字符串里的数字并转换为数值, 用来表示数据区域的宽度
        field_width = -1;
        if ( is_digit( *fmt ) ) {
            field_width = skip_atoi( &fmt );
        }
        // 如果不是数字而是'*', 那么数据区域将由可变参数提供
        else if ( *fmt == '*' ) {
            ++fmt;
            field_width = va_arg( args, int32_t );
            if ( field_width < 0 ) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }
        // 提取出显示数据的精度
        precision = -1;
        if ( *fmt == '.' ) {
            ++fmt;
            if ( is_digit( *fmt ) ) {
                precision = skip_atoi( &fmt );
            }
            else if ( *fmt == '*' ) {
                ++fmt;
                precision = va_arg( args, int32_t );
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
                if ( !( flags & LEFT ) ) {
                    while ( --field_width > 0 ) {
                        *( ++str1 ) = ' ';
                    }
                }
                *str1++ = (char)va_arg( args, int32_t );
                while ( --field_width > 0 ) {
                    *str1++ = ' ';
                }
                break;
            }
            break;
        case 's':
            str2 = va_arg( args, char * );
            if ( !str2 ) {
                str2 = 0;
            }
            for ( auto str_c = str2; *str_c; ++str_c, len++ );

            if ( precision < 0 ) {
                precision = len;
            }
            else if ( len > precision ) {
                len = precision;
            }
            if ( !( flags & LEFT ) ) {
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
                str1 = number( str1, va_arg( args, uint64_t ), 8,
                               field_width, precision, flags );
            }
            else {
                str1 = number( str1, va_arg( args, uint32_t ), 8,
                               field_width, precision, flags );
            }
            break;
        case 'p':
            if ( field_width == -1 ) {
                field_width = 2 * sizeof( void * );
                flags |= ZEROPAD;
            }

            str1 = number( str1,
                           (uint64_t)va_arg( args, void * ),
                           16, field_width, precision, flags );
            break;
        case 'x':
            flags |= static_cast< int32_t >( SMALL );
            goto NextX;
        case 'X':
        NextX:     // 只是为了回避编译器警告"this statement may fall
                   // through",下面也是
            if ( qualifier == 'l' ) {
                str1 = number( str1, va_arg( args, uint64_t ), 16,
                               field_width, precision, flags );
            }
            else {
                str1 = number( str1, va_arg( args, uint32_t ), 16,
                               field_width, precision, flags );
            }
            break;
        case 'd':
        case 'i':
            flags |= SIGN;
            goto NextU;
        case 'u':
        NextU:
            if ( qualifier == 'l' ) {
                str1 = number( str1, va_arg( args, uint64_t ), 10,
                               field_width, precision, flags );
            }
            else {
                str1 = number( str1, va_arg( args, uint32_t ), 10,
                               field_width, precision, flags );
            }
            break;
        case 'b':
            if ( qualifier == 'l' ) {
                str1 = number( str1, va_arg( args, uint64_t ), 2,
                               field_width, precision, flags );
            }
            else {
                str1 = number( str1, va_arg( args, uint32_t ), 2,
                               field_width, precision, flags );
            }
            break;
        case 'n':
            if ( qualifier == 'l' ) {
                auto IP = va_arg( args, int64_t * );
                *IP     = ( str1 - buf );
            }
            else {
                auto IP = va_arg( args, int32_t * );
                *IP     = ( str1 - buf );
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
extern "C" int sprintf( char *buf, const char *format, ... ) {
    va_list args;
    int     val;
    va_start( args, format );
    val = vsprintf( buf, format, args );
    va_end( args );
    return val;
}
extern "C" int vsnprintf( [[maybe_unused]] char *__restrict __s, [[maybe_unused]] size_t __maxlen, [[maybe_unused]] const char *__restrict __format, [[maybe_unused]] va_list __arg ) {
    // TODO : finish the function
    return 0;
}
extern "C" int snprintf( [[maybe_unused]] char *str, [[maybe_unused]] size_t maxlen, [[maybe_unused]] const char *format, ... ) {
    // TODO : finish the function
    return 0;
}