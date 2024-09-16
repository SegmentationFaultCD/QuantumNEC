#include <Boot/utils.hpp>
#include <Library/MemoryAllocationLib.h>
using namespace QuantumNEC::Boot;
auto operator delete( VOID *, unsigned long int ) noexcept -> VOID {
    return;
}
auto operator delete( VOID *address ) noexcept -> VOID {
    if ( !address )
        return;
    gBS->FreePool( address );
    return;
}
auto operator delete[]( VOID *address ) noexcept -> VOID {
    operator delete( address );
    return;
}
auto operator new( unsigned long int size ) noexcept -> VOID * {
    if ( size < 1 ) {
        return nullptr;
    }
    VOID *frame { };
    if ( gBS->AllocatePool( EfiLoaderData, static_cast< uint64_t >( size ), &frame ) != EFI_SUCCESS ) {
        return nullptr;
    }
    return frame;
}
auto operator new[]( unsigned long int cb ) noexcept -> VOID * {
    return operator new( cb );
}
auto operator new( unsigned long int, VOID *address ) noexcept -> VOID * {
    return address;
}