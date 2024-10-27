auto main( int argv, char *argc[], char *envp[] ) -> int;

extern "C" auto __libc_start_main( void ) -> void {
    while ( true );
}
