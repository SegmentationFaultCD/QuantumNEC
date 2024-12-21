#include <print>
auto main( void ) -> int {
    std::println( "{}", ( ( (uint64_t)0x7fffffffffff >> 39 ) & 0x1ff ) );
}