#include <lib/spin_lock.hpp>

using namespace QuantumNEC::Lib;

auto SpinLock::release( VOID ) -> VOID {
    std::atomic_flag_clear_explicit( this, std::memory_order_release );
}
auto SpinLock::acquire( VOID ) -> VOID {
    while ( std::atomic_flag_test_and_set_explicit( this, std::memory_order_acquire ) ) {
        ASM( "pause\n\t" );
    }
}