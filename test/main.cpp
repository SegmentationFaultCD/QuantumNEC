#include <print>

auto main( int, char ** ) -> int {
    long prio_ratios[ 103 ] { };
    prio_ratios[ 0 ] = 128;
    for ( auto i = 1ul; i < 103; ++i ) {
        prio_ratios[ i ] = long( prio_ratios[ i - 1 ] * 1.1 );
        if ( ( prio_ratios[ i - 1 ] * 1.1 - prio_ratios[ i ] ) >= 0.5 ) {
            prio_ratios[ i ]++;
        }
        std::print( "{}\n", prio_ratios[ i ] );
    }
}
