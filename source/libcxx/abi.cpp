auto operator delete( void *, unsigned long int ) noexcept -> void {
}
auto operator delete[]( void *, unsigned long int ) noexcept -> void {
}
auto operator delete( void * ) noexcept -> void {
}
auto operator delete[]( void * ) noexcept -> void {
}
extern "C" {
int __cxa_atexit( void ( * )( void * ), void *, void * ) {
    return 0;
}
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
void __cxa_pure_virtual( ) {
    hcf( );
}
void *__dso_handle;
}
