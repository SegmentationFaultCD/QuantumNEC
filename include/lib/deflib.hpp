#pragma once
#define attribute( x, ... ) __attribute__( ( x, ##__VA_ARGS__ ) )
/**
 * @brief 强制变量在本地分配
 */

#define _volatile [[gnu::volatile]]
#define _asmcall [[gnu::sysv_abi]]
#define _regparms( x ) [[gnu::regparms( x )]]
#define _packed [[gnu::packed]]     // 定义特殊结构
#define _weak [[gnu::weak]]
#define _aligned( n ) alignas( n )
#define _used [[gnu::used]]
#define _unused [[gnu::unused]]

// 特殊标记

#define ASM __asm__ __volatile__
#define container_of( ptr, type, member )                                         \
    ( {                                                                           \
        decltype( ( (type *)0 )->member ) *p = ( ptr );                           \
        (type *)( (unsigned long)p - (unsigned long)&( ( (type *)0 )->member ) ); \
    } )

namespace QuantumNEC::Lib {
constexpr const auto L1_CACHE_BYTES { 32 };
constexpr auto       DIV_ROUND_UP( auto &&X, auto &&STEP ) {
    return ( ( ( X ) + ( STEP - 1 ) ) / STEP );
}

// 将BCD码转换为整数
constexpr auto BCD_TO_BIN( auto &&value ) {
    return ( value & 0xf ) + ( value >> 4 ) * 10;
}
// 将整数转换为BCD码
constexpr auto BIN_TO_BCD( auto &&value ) {
    return ( value / 10 ) * 0x10 + ( value % 10 );
}

constexpr auto SIZEOF_LONG_ALIGN( auto &&size ) {
    return ( size + sizeof( long ) - 1 ) & ~( sizeof( long ) - 1 );
}

}     // namespace QuantumNEC::Lib

#define _C_LINK extern "C"
#define _CXX_LINK extern "C++"
