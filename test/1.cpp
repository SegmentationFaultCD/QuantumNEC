#include <print>

auto main( void ) -> int {
    std::println( "{}", ( 0xffff800002311000 >> 12 ) & 0x1ff );
    std::println( "{:x}", 18446744073709551615ull );
}