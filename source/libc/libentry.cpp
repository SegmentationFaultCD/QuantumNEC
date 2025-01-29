auto main( int argv, char *argc[], char *envp[] ) -> int;

extern "C" auto _start( void ) -> void {
    main( 0, nullptr, nullptr );
}
extern "C" auto __libc_start_main( void ) -> void {
    _start( );
    // 后续处理
    while ( true ) {
    }
}
