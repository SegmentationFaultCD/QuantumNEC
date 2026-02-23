extern "C" int main( void ) {
    __asm__( "syscall" );
    while ( true );

    return 0;
}