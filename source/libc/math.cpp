#include <lib/Uefi.hpp>
#include <libc/math.h>

using namespace QuantumNEC;
PUBLIC _C_LINK double32_t min( IN OUT double32_t a, IN OUT double32_t b ) {
    return a < b ? a : b;
}
PUBLIC _C_LINK double32_t max( IN OUT double32_t a, IN OUT double32_t b ) {
    return a > b ? a : b;
}
/**
 * @brief 求绝对值
 * @param digit 求这个数的绝对值
 */
PUBLIC _C_LINK auto abs( IN double32_t digit ) -> double32_t {
    return digit < 0 ? -digit : digit;
}
PUBLIC _C_LINK auto pow( IN CONST int32_t &__base, IN uint64_t exp ) -> double32_t {
    int32_t result { 1 };
    int32_t base { __base };
    while ( exp ) {
        if ( exp & 1 )
            result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}
PUBLIC _C_LINK auto floor( IN double32_t x ) -> double32_t {
    return ( x - 1 + 0.0000001f ) / 1;
}
PUBLIC _C_LINK auto ceil( IN double32_t x ) -> double32_t {
    return ( x + 1 - 0.0000001f ) / 1;
}
PUBLIC _C_LINK auto round( IN double32_t x ) -> double32_t {
    return ( x > 0 ) ? floor( x + 0.5 ) : ceil( x - 0.5 );
}
PUBLIC _C_LINK auto fmod( IN double32_t x, IN double32_t y ) -> double32_t {
    return x - static_cast< int32_t >( x / y ) * y;
}
PUBLIC _C_LINK auto atan( IN double32_t x ) -> double32_t {
    STATIC CONST uint32_t   sign_mask { 0x80000000 };
    STATIC CONST double32_t b { 0.596227f };
    uint32_t                ux_s { sign_mask & (uint32_t &)x };
    double32_t              bx_a { abs( b * x ) };
    double32_t              num { bx_a + x * x };
    double32_t              atan_1q { num / ( 1.f + bx_a + num ) };
    uint32_t                atan_2q { ux_s | (uint32_t &)atan_1q };
    return (double32_t &)( atan_2q );
}
PUBLIC _C_LINK auto atan2( IN double32_t x, IN double32_t y ) -> double32_t {
    STATIC CONST uint32_t   sign_mask { 0x80000000 };
    STATIC CONST double32_t b { 0.596227f };
    uint32_t                ux_s { sign_mask & (uint32_t &)x };
    uint32_t                uy_s { sign_mask & (uint32_t &)y };
    double32_t              q { (double32_t)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 ) };
    double32_t              bxy_a { abs( b * x * y ) };
    double32_t              num { bxy_a + y * y };
    double32_t              atan_1q { num / ( x * x + bxy_a + num ) };
    uint32_t                uatan_2q { ( ux_s ^ uy_s ) | (uint32_t &)atan_1q };
    return q + (double32_t &)( uatan_2q );
}
PUBLIC _C_LINK auto utof( IN uint32_t x ) -> double32_t {
    union {
        uint64_t i;
        double   d;
    } u;
    u.i = x | ( (uint64_t)0x433 << 52 );
    return u.d - 0x1p52;
}
PUBLIC _C_LINK auto itof( IN int32_t x ) -> double32_t {
    union {
        uint64_t i;
        double   d;
    } u;
    u.i = ( (uint32_t)x ^ 0x80000000 ) | ( (uint64_t)0x433 << 52 );
    return u.d - 0x1.000008p52;
}
PUBLIC _C_LINK auto ulltof( uint64_t x ) -> double32_t {
    union {
        uint64_t i;
        double   d;
    } hi, lo;
    hi.i = ( x >> 32 ) | ( (uint64_t)0x453 << 52 );
    lo.i = ( x & 0xFFFFFFFF ) | ( (uint64_t)0x433 << 52 );
    return hi.d - 0x1.00000001p84 + lo.d;
}
PUBLIC _C_LINK auto lltof( int64_t x ) -> double32_t {
    union {
        uint64_t i;
        double   d;
    } hi, lo;
    hi.i = ( ( x >> 32 ) ^ 0x80000000 ) | ( (uint64_t)0x453 << 52 );
    lo.i = ( x & 0xFFFFFFFF ) | ( (uint64_t)0x433 << 52 );
    return hi.d - 0x1.00000001p84 + lo.d;
}
