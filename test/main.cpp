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
int main( ) {
    uint64_t s { 1 };

    for ( uint64_t a { }; a < 21; ++a ) {
        s += cache_size[ a ];
        std::print( "{}\n", s );
    }

    return 0;
}