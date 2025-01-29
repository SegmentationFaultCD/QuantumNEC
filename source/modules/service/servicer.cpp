#include "../include/lib/quantumnec.h"
auto main( void ) -> int {
    syscall( servicer::RESTART_SYSCALL, 0, 0, 0, 0, 0, 0 );

    return 0;
}
