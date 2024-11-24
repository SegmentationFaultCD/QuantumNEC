auto operator delete( void *, unsigned long int ) noexcept -> void {
}
auto operator delete[]( void *, unsigned long int ) noexcept -> void {
}
auto operator delete( void * ) noexcept -> void {
}
auto operator delete[]( void * ) noexcept -> void {
}
auto operator new( long unsigned int, void *addr ) noexcept -> void * {
    return addr;
}
auto operator new[]( long unsigned int, void *addr ) noexcept -> void * {
    return addr;
}
extern "C" {
void hcf( ) {
    for ( ;; ) {
#if defined( __x86_64__ )
        asm( "hlt" );
#elif defined( __aarch64__ ) || defined( __riscv )
        asm( "wfi" );
#elif defined( __loongarch64 )
        asm( "idle 0" );
#endif
    }
}
int __cxa_atexit( void ( * )( void * ), void *, void * ) {
    hcf( );
    return 0;
}

// void __cxa_pure_virtual( ) {
//     hcf( );
// }
void *__dso_handle;
// void __cxa_finalize( void ) {
//     hcf( );
// }
// void _ITM_deregisterTMCloneTable( void ) {
//     hcf( );
// }
// void _ITM_registerTMCloneTable( void ) {
//     hcf( );
// }
}
