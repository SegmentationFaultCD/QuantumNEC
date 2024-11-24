#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------
//; fhsb 返回 64 位无符号整数中最高有效位的索引，如果没有找到 1 位则返回 -1
#define clz( x )                                               \
    _Generic( ( x ),                                           \
        unsigned char: __builtin_clz( (uint32_t)( x ) ) - 24,  \
        unsigned short: __builtin_clz( (uint32_t)( x ) ) - 16, \
        unsigned int: __builtin_clz( x ),                      \
        unsigned long: __builtin_clzl( x ),                    \
        unsigned long long: __builtin_clzll( x ) )

__ssize_t fhsb8( uint8_t x ) {
    if ( x == 0 )
        return -1;
    return 7 - clz( x );
}
__ssize_t fhsb16( uint16_t x ) {
    if ( x == 0 )
        return -1;
    return 15 - clz( x );
}
__ssize_t fhsb32( uint32_t x ) {
    if ( x == 0 )
        return -1;
    return 31 - clz( x );
}
__ssize_t fhsb64( uint64_t x ) {
    if ( x == 0 )
        return -1;
    return 63 - clz( x );
}
#define fhsb( x )         \
    _Generic( ( x ),      \
        u8: fhsb8( x ),   \
        u16: fhsb16( x ), \
        u32: fhsb32( x ), \
        u64: fhsb64( x ), \
        i8: fhsb8( x ),   \
        i16: fhsb16( x ), \
        i32: fhsb32( x ), \
        i64: fhsb64( x ) )

// --------------------------------------------------
//; 位逆序

uint8_t bit_reverse_8( uint8_t x ) {
    x = ( ( x & 0x55 ) << 1 ) | ( ( x >> 1 ) & 0x55 );
    x = ( ( x & 0x33 ) << 2 ) | ( ( x >> 2 ) & 0x33 );
    x = ( ( x & 0x0f ) << 4 ) | ( ( x >> 4 ) & 0x0f );
    return x;
}
uint16_t bit_reverse_16( uint16_t x ) {
    x = ( ( x & 0x5555 ) << 1 ) | ( ( x >> 1 ) & 0x5555 );
    x = ( ( x & 0x3333 ) << 2 ) | ( ( x >> 2 ) & 0x3333 );
    x = ( ( x & 0x0f0f ) << 4 ) | ( ( x >> 4 ) & 0x0f0f );
    x = ( ( x & 0x00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff );
    return x;
}
uint32_t bit_reverse_32( uint32_t x ) {
    x = ( ( x & 0x55555555 ) << 1 ) | ( ( x >> 1 ) & 0x55555555 );
    x = ( ( x & 0x33333333 ) << 2 ) | ( ( x >> 2 ) & 0x33333333 );
    x = ( ( x & 0x0f0f0f0f ) << 4 ) | ( ( x >> 4 ) & 0x0f0f0f0f );
    x = ( ( x & 0x00ff00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff00ff );
    x = ( x << 16 ) | ( x >> 16 );
    return x;
}
bit_reverse_64( uint64_t x ) {
    x = ( ( x & 0x5555555555555555 ) << 1 ) | ( ( x >> 1 ) & 0x5555555555555555 );
    x = ( ( x & 0x3333333333333333 ) << 2 ) | ( ( x >> 2 ) & 0x3333333333333333 );
    x = ( ( x & 0x0f0f0f0f0f0f0f0f ) << 4 ) | ( ( x >> 4 ) & 0x0f0f0f0f0f0f0f0f );
    x = ( ( x & 0x00ff00ff00ff00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff00ff00ff00ff );
    x = ( ( x & 0x0000ffff0000ffff ) << 16 ) | ( ( x >> 16 ) & 0x0000ffff0000ffff );
    x = ( x << 32 ) | ( x >> 32 );
    return x;
}
#define bit_reverse( x )          \
    _Generic( ( x ),              \
        u8: bit_reverse_8( x ),   \
        u16: bit_reverse_16( x ), \
        u32: bit_reverse_32( x ), \
        u64: bit_reverse_64( x ), \
        i8: bit_reverse_8( x ),   \
        i16: bit_reverse_16( x ), \
        i32: bit_reverse_32( x ), \
        i64: bit_reverse_64( x ) )

// --------------------------------------------------
//; 字节逆序

finline u8 byteswap8( u8 x ) {
    return x;
}
finline u16 byteswap16( u16 x ) {
    x = ( ( x & 0x00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff );
    return x;
}
finline u32 byteswap32( u32 x ) {
    x = ( ( x & 0x00ff00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff00ff );
    x = ( x << 16 ) | ( x >> 16 );
    return x;
}
finline u64 byteswap64( u64 x ) {
    x = ( ( x & 0x00ff00ff00ff00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff00ff00ff00ff );
    x = ( ( x & 0x0000ffff0000ffff ) << 16 ) | ( ( x >> 16 ) & 0x0000ffff0000ffff );
    x = ( x << 32 ) | ( x >> 32 );
    return x;
}
#define byteswap( x )         \
    _Generic( ( x ),          \
        u8: byteswap8( x ),   \
        u16: byteswap16( x ), \
        u32: byteswap32( x ), \
        u64: byteswap64( x ), \
        i8: byteswap8( x ),   \
        i16: byteswap16( x ), \
        i32: byteswap32( x ), \
        i64: byteswap64( x ) )
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define little_endian( x ) ( x )
#define big_endian( x ) ( byteswap( x ) )
#else
#define little_endian( x ) ( byteswap( x ) )
#define big_endian( x ) ( x )
#endif

#ifdef __cplusplus
}
#endif