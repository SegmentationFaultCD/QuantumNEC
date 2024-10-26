#pragma once
#define attribute( x, ... ) __attribute__( ( x, ##__VA_ARGS__ ) )
/**
 * @brief 强制变量在本地分配
 */

#define _volatile __attribute__( ( volatile ) )
#define _asmcall __attribute__( ( sysv_abi ) )
#define _regparms( x ) __attribute__( regparms( x ) )
#define _packed __attribute__( ( packed ) )     // 定义特殊结构
#define _weak __attribute__( ( weak ) )
#define _aligned( n ) __attribute__( ( __aligned__( ( n ) ) ) )
#define _used __attribute__( ( used ) )
#define _unused __attribute__( ( unused ) )
#pragma once
#define VOID void
#define CONST const
#define THIS this
// 特殊标记
#define PUBLIC
#define PRIVATE static
#define __PRIVATE__ static
#define _PRIVATE_DATA_ static
#define _PRIVATE_FUNC_ static
#define STATIC static
#define EXTERN extern
#define ASM __asm__ __volatile__
#define container_of( ptr, type, member )                                         \
    ( {                                                                           \
        decltype( ( (type *)0 )->member ) *p = ( ptr );                           \
        (type *)( (unsigned long)p - (unsigned long)&( ( (type *)0 )->member ) ); \
    } )

PUBLIC namespace QuantumNEC::Lib {
    PUBLIC constexpr auto CHECKFLAG( const auto &field, const auto &flag ) {
        return field & flag;
    }

    PUBLIC constexpr const auto CRLF { "\r\n" };
    PUBLIC constexpr auto IS_DIGIT( const auto &c ) {
        return c >= '0' && c <= '9';
    };
    PUBLIC constexpr auto BIT( const auto &x ) {
        return (unsigned long long)( 1 ) << ( x );
    }
    PUBLIC constexpr const auto L1_CACHE_BYTES { 32 };
    PUBLIC constexpr auto DIV_ROUND_UP( const auto &X, const auto &STEP ) {
        return ( ( ( X ) + ( STEP - 1 ) ) / STEP );
    }

    PUBLIC constexpr auto OFFSET( const auto *address ) {
        return (unsigned long long)( address ) & 0x1FFFFF;
    }

    // 将BCD码转换为整数
    PUBLIC constexpr auto BCD_TO_BIN( auto value ) {
        return ( value & 0xf ) + ( value >> 4 ) * 10;
    }

    // 将整数转换为BCD码
    PUBLIC constexpr auto BIN_TO_BCD( auto value ) {
        return ( value / 10 ) * 0x10 + ( value % 10 );
    }
    PUBLIC constexpr auto SIZEOF_LONG_ALIGN( auto size ) {
        return ( size + sizeof( long ) - 1 ) & ~( sizeof( long ) - 1 );
    }

}     // namespace QuantumNEC::Lib::Base

#if defined( __x86_64__ )
#define _BARRIER "mfence\n\t"
#define BARRIER ASM( _BARRIER );
#define DO_DIV( n, base )                             \
    ( {                                               \
        int __res;                                    \
        __asm__( "divq %%rcx"                         \
                 : "=a"( n ), "=d"( __res )           \
                 : "0"( n ), "1"( 0 ), "c"( base ) ); \
        __res;                                        \
    } )

#elif defined( __aarch64__ )
#else
#error Not any defines
#endif
#define OFFSETOF( type, member ) __builtin_offsetof( type, member )
#define _C_LINK extern "C"
#define _CXX_LINK extern "C++"
