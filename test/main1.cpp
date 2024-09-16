module;
#include <print>
#include <utility>
export module Main;
using arr = char[ 1024 ];
using str = char[ 1024 ];
export auto main( void ) -> int {
    std::pair< arr, str > s;
    sizeof s;
    return 0;
}