#include <bits/stdc++.h>
#include <print>
constexpr auto __zone_min_memory__ = 0x200000ul * 1024ul;

auto main( void ) -> int {
    constexpr auto pow = []( this auto &self, u_int64_t x ) consteval {
        if ( x == 1 )
            return 0;
        else
            return 1 + self( x >> 1 );
    };

    std::println( "{:b}", pow( __zone_min_memory__ ) );
    std::println( "{:b}", 0x80000000ul * 12ul == ( 0x6f0200000ul & ( 0x80000000ul - 1 ) ) );
    ( 0x114200000 >> 21ul ) & 0b1111;

    // auto s = ( 0xffff800001800000 & ( ~( 0x200000ul - 1ul ) ) );
    // 0001 0010 0000 0000 0000 0000 0000
    //                          0001 0010
    // 0010 0000 0000 0000 0000 0000
}