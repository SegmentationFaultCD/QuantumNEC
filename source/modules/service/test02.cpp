auto main( void ) -> int {
    while ( true ) {
        __asm__( "movq $0, %rax" );
        __asm__( "movq $0x0a, %rdi" );
        __asm__( "syscall" );
    }
    return 0;
}
