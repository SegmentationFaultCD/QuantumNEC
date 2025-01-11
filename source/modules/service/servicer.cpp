auto main( void ) -> int {
    __asm__( "movq $0, %rax" );
    __asm__( "movq $11, %rdi" );
    __asm__( "syscall" );
    while ( true ) {
    }
    return 0;
}
