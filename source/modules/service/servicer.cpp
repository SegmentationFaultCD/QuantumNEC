#include "../include/lib/quantumnec.h"

extern "C" auto main( void ) -> int {
    while ( true ) {
        syscall_entry( servicer::RESTART_SYSCALL, 0, 0, 0, 0, 0, 0 );
    }
    return 0;
}
