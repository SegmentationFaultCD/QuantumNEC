#include <libc/math.h>

extern "C" float min( float a, float b ) {
    return a < b ? a : b;
}
extern "C" float max( float a, float b ) {
    return a > b ? a : b;
}
/**
 * @brief 求绝对值
 * @param digit 求这个数的绝对值
 */
extern "C" auto abs( float digit ) -> float {
    return digit < 0 ? -digit : digit;
}
extern "C" auto pow( const int32_t &__base, uint64_t exp ) -> float {
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
extern "C" auto floor( float x ) -> float {
    return ( x - 1 + 0.0000001f ) / 1;
}
extern "C" auto ceil( float x ) -> float {
    return ( x + 1 - 0.0000001f ) / 1;
}
extern "C" auto round( float x ) -> float {
    return ( x > 0 ) ? floor( x + 0.5 ) : ceil( x - 0.5 );
}
extern "C" auto fmod( float x, float y ) -> float {
    return x - static_cast< int32_t >( x / y ) * y;
}
extern "C" auto atan( float x ) -> float {
    constexpr uint32_t sign_mask { 0x80000000 };
    constexpr float    b { 0.596227f };
    uint32_t           ux_s { sign_mask & (uint32_t &)x };
    float              bx_a { abs( b * x ) };
    float              num { bx_a + x * x };
    float              atan_1q { num / ( 1.f + bx_a + num ) };
    uint32_t           atan_2q { ux_s | (uint32_t &)atan_1q };
    return (float &)( atan_2q );
}
extern "C" auto atan2( float x, float y ) -> float {
    constexpr uint32_t sign_mask { 0x80000000 };
    constexpr float    b { 0.596227f };
    uint32_t           ux_s { sign_mask & (uint32_t &)x };
    uint32_t           uy_s { sign_mask & (uint32_t &)y };
    float              q { (float)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 ) };
    float              bxy_a { abs( b * x * y ) };
    float              num { bxy_a + y * y };
    float              atan_1q { num / ( x * x + bxy_a + num ) };
    uint32_t           uatan_2q { ( ux_s ^ uy_s ) | (uint32_t &)atan_1q };
    return q + (float &)( uatan_2q );
}
extern "C" auto utof( uint32_t x ) -> float {
    union {
        uint64_t i;
        double   d;
    } u;
    u.i = x | ( (uint64_t)0x433 << 52 );
    return u.d - 0x1p52;
}
extern "C" auto itof( int32_t x ) -> float {
    union {
        uint64_t i;
        double   d;
    } u;
    u.i = ( (uint32_t)x ^ 0x80000000 ) | ( (uint64_t)0x433 << 52 );
    return u.d - 0x1.000008p52;
}
extern "C" auto ulltof( uint64_t x ) -> float {
    union {
        uint64_t i;
        double   d;
    } hi, lo;
    hi.i = ( x >> 32 ) | ( (uint64_t)0x453 << 52 );
    lo.i = ( x & 0xFFFFFFFF ) | ( (uint64_t)0x433 << 52 );
    return hi.d - 0x1.00000001p84 + lo.d;
}
extern "C" auto lltof( int64_t x ) -> float {
    union {
        uint64_t i;
        double   d;
    } hi, lo;
    hi.i = ( ( x >> 32 ) ^ 0x80000000 ) | ( (uint64_t)0x453 << 52 );
    lo.i = ( x & 0xFFFFFFFF ) | ( (uint64_t)0x433 << 52 );
    return hi.d - 0x1.00000001p84 + lo.d;
}
