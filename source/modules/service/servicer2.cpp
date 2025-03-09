#include "../include/lib/quantumnec.h"

auto main( void ) -> int {
    while ( true ) {
        syscall_entry( servicer::RESTART_SYSCALL, 1, 0, 0, 0, 0, 0 );
    }
    return 0;
}
