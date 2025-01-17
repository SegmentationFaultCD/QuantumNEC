auto main( void ) -> int {
    while ( true ) {
        __asm__( "movq $0, %rax" );
        __asm__( "movq $0x0c, %rdi" );
        __asm__( "syscall" );
    }
    return 0;
}
