#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/kernel.hpp>
using namespace QuantumNEC::Kernel;

auto operator new( IN unsigned long int size ) -> VOID * {
    return ( Memory::heap->malloc( size ) );
}
auto operator new[]( IN unsigned long int size ) -> VOID * {
    return ( Memory::heap->malloc( size ) );
}

auto operator delete( IN VOID *address ) noexcept -> VOID {
    Memory::heap->free( ( address ) );
}
auto operator delete[]( IN VOID *address ) noexcept -> VOID {
    Memory::heap->free( ( address ) );
}
