#include <print>
#include <stdio.h>
constexpr uint64_t cache_size[] {
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
    65536,
    131072,
    262144,
    524288,
    1048576,

};
constexpr auto DIV_ROUND_UP( const auto &X, const auto &STEP ) {
    return ( ( ( X ) + ( STEP - 1 ) ) / STEP );
}
int main( ) {
    uint64_t s { 1 };
    auto     i = 11;
    auto     j = 5;
    for ( uint64_t a { }; a < 21; ++a ) {
        s += cache_size[ a ];
        std::print( "{}\n", s );
    }
    std::print( "{}\n", DIV_ROUND_UP( i, j ) );
    return 0;
}